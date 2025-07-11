#include "WavetableOscillator.h"

// Ctor: expects an unique name for the waveform
// This name is used for managiong wavetable files
WavetableOscillator::WavetableOscillator(const std::string formName)
{
    // to avoid vtable lookup in DSPObject
    registerBlockProcessor(&WavetableOscillator::processBlock);

    // set the waveform name
    waveformName = formName;

    // Define the base frequencies at which a new wavetable is used
    // These roughly correspond to octave steps starting from 20 Hz
    baseFrequencies = {20.0, 40.0, 160.0, 640.0, 2560.0};

    // Define the corresponding table size for each frequency range
    // Higher frequencies require higher resolution to avoid interpolation artifacts
    tableSizes = {1024, 2048, 4096, 8192, 16384};

    // Initialize wavetable buffers
    wavetableCalcBuffers.reserve(tableSizes.size());
    wavetableSampleBuffers.reserve(tableSizes.size());
}

// Destructor releases memory for wavetables
WavetableOscillator::~WavetableOscillator()
{
    wavetableCalcBuffers.clear();
}

void WavetableOscillator::initialize()
{
    DSPObject::initialize();

    outputBufferL.create(DSP::blockSize);
    outputBufferR.create(DSP::blockSize);
    modulationBufferL.create(DSP::blockSize);
    modulationBufferR.create(DSP::blockSize);

    setFrequency(0.0);
    setModIndex(0);
    setNumVoices(1);
    setDetune(0.03);
    resetPhase();

    lastFrequency = -1.0;

    DSP::log("Loading wavetable for %s", waveformName.c_str());

    if (!load())
    {
        DSP::log("Wavetable for %s does not exist: generating...", waveformName.c_str());

        for (size_t i = 0; i < tableSizes.size(); ++i)
        {
            size_t size = tableSizes[i];
            dsp_float freq = baseFrequencies[i];

            // Create a new DSPBuffer instance and resize it to the desired table size
            DSPBuffer* buffer = new DSPBuffer();
            buffer->create(size);

            // Let the subclass generate the actual waveform data
            createWavetable(*buffer, freq);

            // Store the buffer for later use 
            wavetableCalcBuffers.push_back(buffer);
        }

        DSP::log("Wavetable for %s generated: saving...", waveformName.c_str());
        save();
        DSP::log("Wavetable for %s generated saved, loading...", waveformName.c_str());

        // load and clear
        load();
        wavetableSampleBuffers.clear();
    }
}

// Gets the current frequency
dsp_float WavetableOscillator::getFrequency()
{
    return frequency;
}

// Sets the frequency
void WavetableOscillator::setFrequency(dsp_float value)
{
    if (value == frequency)
        return;

    frequency = clampmin(value, 0.0);
}

// Sets the modulation index for frequency modulation.
// This controls the intensity of the frequency modulation effect.
void WavetableOscillator::setModIndex(dsp_float index)
{
    modulationIndex = clamp(index, 0.0, 100);
}

void WavetableOscillator::setNumVoices(int count)
{
    // Clamp to [1, 9] and resize
    numVoices = clamp(count, 1, 9);
    voices.resize(numVoices);

    for (int i = 0; i < numVoices; ++i)
    {
        // Randomize phase [0.0, 1.0)
        voices[i].phase = static_cast<dsp_float>(rand()) / RAND_MAX;

        // Stereo panning - from -1.0 (left) to +1.0 (right)
        dsp_float pan = (numVoices > 1)
                            ? static_cast<dsp_float>(i) / (numVoices - 1) * 2.0 - 1.0
                            : 0.0;

        voices[i].gainL = std::sqrt(0.5 * (1.0 - pan));
        voices[i].gainR = std::sqrt(0.5 * (1.0 + pan));
    }

    // Normalize amplitude across voices
    for (int i = 0; i < numVoices; ++i)
        voices[i].amp_ratio = 3.5 / numVoices;

    updateDetune(); // ensure detune_ratios match after resizing
}

void WavetableOscillator::updateDetune()
{
    // Detune spread from -1.0 to +1.0
    dsp_float center = (numVoices - 1) / 2.0;
    for (int i = 0; i < numVoices; ++i)
    {
        dsp_float offset = i - center;
        voices[i].detune_ratio = detune * offset / center;
    }
}

void WavetableOscillator::setDetune(dsp_float value)
{
    detune = clamp(value, 0.0, 1.0) * 0.125;
    updateDetune();
}

void WavetableOscillator::selectTable(double frequency)
{
    for (size_t i = baseFrequencies.size(); i-- > 0;)
    {
        if (frequency >= baseFrequencies[i])
        {
            selectedWaveTable = wavetableSampleBuffers[i];
            selectedWaveTableSize = selectedWaveTable->size();
        }
    }

    // Fallback
    selectedWaveTable = wavetableSampleBuffers.front();
    selectedWaveTableSize = selectedWaveTable->size();
}

// Returns true if the oscillator's phase wrapped during the last getSample() call
bool WavetableOscillator::hasWrapped()
{
    return wrapped;
}

// Resets the wrap status
void WavetableOscillator::unWrap()
{
    wrapped = false;
}

// Resets the internal oscillator phase to 0.0.
void WavetableOscillator::resetPhase()
{
    currentPhase = 0.0;
    wrapped = false;
}

// Next sample block generation
void WavetableOscillator::processBlock(DSPObject *dsp)
{
    WavetableOscillator *osc = static_cast<WavetableOscillator *>(dsp);

    dsp_float phase = osc->currentPhase;
    bool wrappedFlag = false;

    // Select wavetable once per sample block
    if (osc->frequency != osc->lastFrequency)
    {
        osc->selectTable(osc->frequency);
        osc->lastFrequency = osc->frequency;
    }

    const DSPSampleBuffer &waveTable = *(osc->selectedWaveTable);
    size_t waveTableSize = osc->selectedWaveTableSize;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        dsp_float modLeft = osc->modulationBufferL[i];
        dsp_float modRight = osc->modulationBufferR[i];

        if (osc->numVoices > 1)
        {
            dsp_float sumL = 0.0;
            dsp_float sumR = 0.0;

            for (auto &v : osc->voices)
            {
                dsp_float voiceFreq = osc->frequency * (1.0 + v.detune_ratio);

                // Modulation
                dsp_float modSignal = (v.gainL > v.gainR) ? modLeft : modRight;

                // Phase modulation
                dsp_float modulatedPhase = v.phase + osc->modulationIndex * modSignal;
                modulatedPhase -= std::floor(modulatedPhase); // Wrap to [0, 1)

                dsp_float index = modulatedPhase * waveTableSize;
                size_t i0 = static_cast<size_t>(index);
                size_t i1 = (i0 + 1) % waveTableSize;
                dsp_float frac = index - i0;

                // Interpolation
                dsp_float sample = (1.0 - frac) * waveTable[i0] + frac * waveTable[i1];

                // Sum weighted
                sumL += sample * v.amp_ratio * v.gainL;
                sumR += sample * v.amp_ratio * v.gainR;

                // Advance phase
                dsp_float inc = voiceFreq / DSP::sampleRate;
                v.phase += inc;

                if (v.phase >= 1.0)
                {
                    v.phase -= 1.0;
                    wrappedFlag = true;
                }
            }

            osc->outputBufferL[i] = sumL;
            osc->outputBufferR[i] = sumR;
        }
        else
        {
            phase += osc->phaseIncrement;

            if (phase >= 1.0)
            {
                phase -= 1.0;
                wrappedFlag = true;
            }

            dsp_float modPhaseL = phase + osc->modulationIndex * modLeft;
            dsp_float modPhaseR = phase + osc->modulationIndex * modRight;

            modPhaseL -= std::floor(modPhaseL);
            modPhaseR -= std::floor(modPhaseR);

            dsp_float indexL = modPhaseL * osc->selectedWaveTableSize;
            dsp_float indexR = modPhaseR * osc->selectedWaveTableSize;

            size_t i0L = static_cast<size_t>(indexL);
            size_t i1L = (i0L + 1) % osc->selectedWaveTableSize;
            dsp_float fracL = indexL - i0L;

            size_t i0R = static_cast<size_t>(indexR);
            size_t i1R = (i0R + 1) % osc->selectedWaveTableSize;
            dsp_float fracR = indexR - i0R;

            osc->outputBufferL[i] = (1.0 - fracL) * (*osc->selectedWaveTable)[i0L] + fracL * (*osc->selectedWaveTable)[i1L];
            osc->outputBufferR[i] = (1.0 - fracR) * (*osc->selectedWaveTable)[i0R] + fracR * (*osc->selectedWaveTable)[i1R];
        }
    }

    osc->currentPhase = phase;
    osc->wrapped = wrappedFlag;
}

static void createDir()
{
    // Check if directory exists
    if (access("tables", F_OK) == -1)
    {
        // Create directory with rwx------ (0700), adjust if needed
        mkdir("tables", 0700);
    }
}

static std::string absolutePath(const std::string &relativePath)
{
    char fullPath[PATH_MAX];
    if (realpath(relativePath.c_str(), fullPath) != nullptr)
    {
        return std::string(fullPath);
    }
    else
    {
        return relativePath; // fallback
    }
}

bool WavetableOscillator::load()
{
    std::string fileName = "tables/" + waveformName + "_" + std::to_string(static_cast<int>(DSP::sampleRate)) + ".wave";

    DSP::log("Try loading wavetable %s", absolutePath(fileName).c_str());

    std::ifstream inFile(fileName.c_str());
    if (!inFile.is_open())
        return false;

    DSP::log("Found wavetable %s", absolutePath(fileName).c_str());

    wavetableSampleBuffers.clear();
    baseFrequencies.clear();
    tableSizes.clear();

    std::string line;
    while (std::getline(inFile, line))
    {
        std::stringstream ss(line);
        std::string item;

        try
        {
            // Read frequency
            if (!std::getline(ss, item, ','))
                continue;

            host_float freq = std::stod(item);

            // Read size
            if (!std::getline(ss, item, ','))
                continue;

            size_t size = static_cast<size_t>(std::stoul(item));

            DSPSampleBuffer* buffer = new DSPSampleBuffer();;
            buffer->create(size);

            // Read data
            size_t sampleCount = 0;

            while (std::getline(ss, item, ',') && sampleCount < size)
            {
                (*buffer)[sampleCount++] = static_cast<host_float>(std::stod(item));
            }

            if (sampleCount != size)
            {
                DSP::log("Invalid format in wavetable %s", absolutePath(fileName).c_str());
                return false;
            }

            baseFrequencies.push_back(freq);
            tableSizes.push_back(size);
            wavetableSampleBuffers.push_back(buffer);
        }
        catch (const std::exception &ex)
        {
            DSP::log("Error generating wave forms from wavetable %s (%s)", absolutePath(fileName), ex.what());
            return false;
        }
        catch (...)
        {
            DSP::log("Error reading wavetable %s", absolutePath(fileName).c_str());
            return false;
        }
    }

    DSP::log("Wavetable %s loaded", absolutePath(fileName).c_str());

    return !wavetableSampleBuffers.empty();
}

void WavetableOscillator::save() const
{
    createDir();

    std::string fileName = "tables/" + waveformName + "_" + std::to_string(static_cast<int>(DSP::sampleRate)) + ".wave";
    std::ofstream outFile(fileName.c_str());

    if (!outFile.is_open())
    {
        DSP::log("Wavetable %s does not exist", absolutePath(fileName).c_str());
        return;
    }

    try
    {
        for (size_t i = 0; i < wavetableCalcBuffers.size(); ++i)
        {
            const DSPBuffer &buffer = *wavetableCalcBuffers[i];
            outFile << baseFrequencies[i] << "," << buffer.size();

            for (size_t j = 0; j < buffer.size(); ++j)
            {
                outFile << "," << buffer[j];
            }

            outFile << "\n";
        }
    }
    catch (const std::exception &ex)
    {
        DSP::log("Error writing wave form to wavetable %s (%s)", absolutePath(fileName).c_str(), ex.what());
    }
    catch (...)
    {
        DSP::log("Error writung wave form to wavetable %s", absolutePath(fileName).c_str());
    }
}
