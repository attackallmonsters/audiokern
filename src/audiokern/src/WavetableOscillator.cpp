#include "WavetableOscillator.h"

std::vector<SharedWavetableSet> WavetableOscillator::sharedWavetables;

// Ctor: expects an unique name for the waveform
// This name is used for managiong wavetable files
WavetableOscillator::WavetableOscillator(const std::string formName)
{
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
    wavetableSampleBuffers.clear();
}

void WavetableOscillator::initializeGenerator()
{
    setFrequency(0.0);
    setModIndex(0);
    setNumVoices(1);
    setDetune(0.03);
    resetPhase();
    setAnalogDrift(0.0);

    lastFrequency = -1.0;

    acquireSharedWavetable();
}

// Gets the current frequency
host_float WavetableOscillator::getFrequency()
{
    return frequency;
}

// Sets the frequency
void WavetableOscillator::setFrequency(host_float value)
{
    if (value == frequency)
        return;

    frequency = clampmin(value, 0.0);
}

// Sets the modulation index for frequency modulation.
// This controls the intensity of the frequency modulation effect.
void WavetableOscillator::setModIndex(host_float index)
{
    modulationIndex = clamp(index, 0.0, 100);
}

void WavetableOscillator::setNumVoices(int count)
{
    // Clamp to [1, 9] and resize
    numVoices = clamp(count, 1, 9);
    voices.resize(numVoices);

    // to avoid vtable lookup in DSPObject
    if (count == 1)
        registerBlockProcessor(&WavetableOscillator::processBlockVoice);
    else
        registerBlockProcessor(&WavetableOscillator::processBlockVoices);

    for (int i = 0; i < numVoices; ++i)
    {
        // Randomize phase [0.0, 1.0)
        voices[i].phase = static_cast<host_float>(rand()) / RAND_MAX;

        // Stereo panning - from -1.0 (left) to +1.0 (right)
        host_float pan = (numVoices > 1)
                            ? static_cast<host_float>(i) / (numVoices - 1) * 2.0 - 1.0
                            : 0.0;

        voices[i].gainL = std::sqrt(0.5 * (1.0 - pan));
        voices[i].gainR = std::sqrt(0.5 * (1.0 + pan));
    }

    // Normalize amplitude across voices
    for (int i = 0; i < numVoices; ++i)
        voices[i].amp_ratio = 3.5 / numVoices;

    updateDetune(); // ensure detune_ratios match after resizing

    voiceGain = getVocieGain(numVoices);
}

void WavetableOscillator::updateDetune()
{
    // Detune spread from -1.0 to +1.0
    host_float center = (numVoices - 1) / 2.0;
    for (int i = 0; i < numVoices; ++i)
    {
        host_float offset = i - center;
        voices[i].detune_ratio = detune * offset / center;
    }
}

void WavetableOscillator::setDetune(host_float value)
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

void WavetableOscillator::setAnalogDrift(host_float d)
{
    drift = d;
}

// Next sample block generation one voice
void WavetableOscillator::processBlockVoice()
{
    // Select wavetable once per sample block
    if (frequency != lastFrequency)
    {
        selectTable(frequency);
        lastFrequency = frequency;
    }

    phaseIncrement = (frequency + drift) / DSP::sampleRate;

    if (generatorRole == GeneratorRole::Normal)
    {
        for (size_t i = 0; i < DSP::blockSize; ++i)
        {
            currentPhase += phaseIncrement;

            if (currentPhase >= 1.0)
            {
                currentPhase -= 1.0;
                wrapped = true;
            }

            host_float modPhaseL = currentPhase;
            host_float modPhaseR = currentPhase;

            modPhaseL -= std::floor(modPhaseL);
            modPhaseR -= std::floor(modPhaseR);

            host_float indexL = modPhaseL * selectedWaveTableSize;
            host_float indexR = modPhaseR * selectedWaveTableSize;

            size_t i0L = static_cast<size_t>(indexL);
            size_t i1L = (i0L + 1) % selectedWaveTableSize;
            host_float fracL = indexL - i0L;

            size_t i0R = static_cast<size_t>(indexR);
            size_t i1R = (i0R + 1) % selectedWaveTableSize;
            host_float fracR = indexR - i0R;

            outputBus->l[i] = (1.0 - fracL) * (*selectedWaveTable)[i0L] + fracL * (*selectedWaveTable)[i1L];
            outputBus->r[i] = (1.0 - fracR) * (*selectedWaveTable)[i0R] + fracR * (*selectedWaveTable)[i1R];
        }
    }
    else
    {
        for (size_t i = 0; i < DSP::blockSize; ++i)
        {
            host_float modLeft = fmBus->l[i];
            host_float modRight = fmBus->r[i];

            currentPhase += phaseIncrement;

            if (currentPhase >= 1.0)
            {
                currentPhase -= 1.0;
                wrapped = true;
            }

            host_float modPhaseL = currentPhase + modulationIndex * modLeft;
            host_float modPhaseR = currentPhase + modulationIndex * modRight;

            modPhaseL -= std::floor(modPhaseL);
            modPhaseR -= std::floor(modPhaseR);

            host_float indexL = modPhaseL * selectedWaveTableSize;
            host_float indexR = modPhaseR * selectedWaveTableSize;

            size_t i0L = static_cast<size_t>(indexL);
            size_t i1L = (i0L + 1) % selectedWaveTableSize;
            host_float fracL = indexL - i0L;

            size_t i0R = static_cast<size_t>(indexR);
            size_t i1R = (i0R + 1) % selectedWaveTableSize;
            host_float fracR = indexR - i0R;

            outputBus->l[i] = (1.0 - fracL) * (*selectedWaveTable)[i0L] + fracL * (*selectedWaveTable)[i1L];
            outputBus->r[i] = (1.0 - fracR) * (*selectedWaveTable)[i0R] + fracR * (*selectedWaveTable)[i1R];
        }
    }
}

void WavetableOscillator::processBlockVoices()
{
    // Select wavetable once per sample block
    if (frequency != lastFrequency)
    {
        selectTable(frequency);
        lastFrequency = frequency;
    }

    if (generatorRole == GeneratorRole::Normal)
    {
        for (size_t i = 0; i < DSP::blockSize; ++i)
        {
            host_float sumL = 0.0;
            host_float sumR = 0.0;

            for (auto &v : voices)
            {
                host_float voiceFreq = (frequency + drift) * (1.0 + v.detune_ratio);

                // Phase modulation
                host_float phase = v.phase;
                phase -= std::floor(phase); // Wrap to [0, 1)

                host_float index = phase * selectedWaveTableSize;
                size_t i0 = static_cast<size_t>(index);
                size_t i1 = (i0 + 1) % selectedWaveTableSize;
                host_float frac = index - i0;

                // Interpolation
                host_float sample = (1.0 - frac) * (*selectedWaveTable)[i0] + frac * (*selectedWaveTable)[i1];

                // Sum weighted
                sumL += sample * v.amp_ratio * v.gainL;
                sumR += sample * v.amp_ratio * v.gainR;

                // Advance phase
                host_float inc = voiceFreq / DSP::sampleRate;
                v.phase += inc;

                if (v.phase >= 1.0)
                {
                    v.phase -= 1.0;
                    wrapped = true;
                }
            }

            outputBus->l[i] = sumL * voiceGain;
            outputBus->r[i] = sumR * voiceGain;
        }
    }
    else
    {
        for (size_t i = 0; i < DSP::blockSize; ++i)
        {
            host_float modLeft = fmBus->l[i];
            host_float modRight = fmBus->r[i];

            host_float sumL = 0.0;
            host_float sumR = 0.0;

            for (auto &v : voices)
            {
                host_float voiceFreq = (frequency + drift) * (1.0 + v.detune_ratio);

                // Modulation
                host_float modSignal = (v.gainL > v.gainR) ? modLeft : modRight;

                // Phase modulation
                host_float modulatedPhase = v.phase + modulationIndex * modSignal;
                modulatedPhase -= std::floor(modulatedPhase); // Wrap to [0, 1)

                host_float index = modulatedPhase * selectedWaveTableSize;
                size_t i0 = static_cast<size_t>(index);
                size_t i1 = (i0 + 1) % selectedWaveTableSize;
                host_float frac = index - i0;

                // Interpolation
                host_float sample = (1.0 - frac) * (*selectedWaveTable)[i0] + frac * (*selectedWaveTable)[i1];

                // Sum weighted
                sumL += sample * v.amp_ratio * v.gainL;
                sumR += sample * v.amp_ratio * v.gainR;

                // Advance phase
                host_float inc = voiceFreq / DSP::sampleRate;
                v.phase += inc;

                if (v.phase >= 1.0)
                {
                    v.phase -= 1.0;
                    wrapped = true;
                }
            }

            outputBus->l[i] = sumL * voiceGain;
            outputBus->r[i] = sumR * voiceGain;
        }
    }
}

// Next sample block generation one voice
void WavetableOscillator::processBlockVoice(DSPObject *dsp)
{
    WavetableOscillator *self = static_cast<WavetableOscillator *>(dsp);
    self->processBlockVoice();
}

// Next sample block generation multiple voices
void WavetableOscillator::processBlockVoices(DSPObject *dsp)
{
    WavetableOscillator *self = static_cast<WavetableOscillator *>(dsp);
    self->processBlockVoices();
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

void WavetableOscillator::acquireSharedWavetable()
{
#if DEBUG
    DSP::log("Loading wavetable for %s", waveformName.c_str());
#endif

    // Step 1: Already in cache?
    for (const auto &entry : sharedWavetables)
    {
        if (entry.name == waveformName)
        {
            baseFrequencies = entry.baseFrequencies;
            tableSizes = entry.tableSizes;
            wavetableSampleBuffers = entry.buffers;

            return;
        }
    }

    // Step 2: Try to load
    if (!load())
    {
#if DEBUG
        DSP::log("Wavetable for %s does not exist: generating...", waveformName.c_str());
#endif

        wavetableCalcBuffers.clear();

        // Prepare base frequencies and table sizes
        baseFrequencies = {20.0, 40.0, 160.0, 640.0, 2560.0};
        tableSizes = {1024, 2048, 4096, 8192, 16384};

        for (size_t i = 0; i < tableSizes.size(); ++i)
        {
            size_t size = tableSizes[i];
            host_float freq = baseFrequencies[i];

            DSPBuffer *buffer = new DSPBuffer();
            buffer->create(size);
            createWavetable(*buffer, freq);
            wavetableCalcBuffers.push_back(buffer);
        }

#if DEBUG
        DSP::log("Wavetable for %s generated: saving...", waveformName.c_str());
#endif
        save();
    }

    // Step 3: Load final result
    if (!load())
    {
        DSP::log("Failed to load wavetable for %s after creation", waveformName.c_str());
        return;
    }

    // Step 4: Register in shared set
    SharedWavetableSet newEntry;
    newEntry.name = waveformName;
    newEntry.baseFrequencies = baseFrequencies;
    newEntry.tableSizes = tableSizes;
    newEntry.buffers = wavetableSampleBuffers;

    sharedWavetables.push_back(newEntry);
#if DEBUG
    DSP::log("Wavetable for %s cached globally", waveformName.c_str());
#endif
}

bool WavetableOscillator::load()
{
    std::string fileName = "tables/" + waveformName + "_" + std::to_string(static_cast<int>(DSP::sampleRate)) + ".wave";
#if DEBUG
    DSP::log("Try loading wavetable %s", absolutePath(fileName).c_str());
#endif

    std::ifstream inFile(fileName.c_str());
    if (!inFile.is_open())
        return false;

#if DEBUG
    DSP::log("Found wavetable %s", absolutePath(fileName).c_str());
#endif

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

            DSPSampleBuffer *buffer = new DSPSampleBuffer();

            buffer->initialize("buffer" + getName(), size);

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
#if DEBUG
    DSP::log("Wavetable %s loaded", absolutePath(fileName).c_str());
#endif

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

host_float WavetableOscillator::getVocieGain(int numVoices)
{
    // Empirically determined values
    switch (numVoices)
    {
    case 1:
        return 1.0;
    case 2:
        return 0.55;
    case 3:
        return 0.55;
    case 4:
        return 0.65;
    case 5:
        return 0.7;
    case 6:
        return 0.75;
    case 7:
        return 0.8;
    case 8:
        return 0.9;
    case 9:
        return 1.0;
    default:
        return 1.0;
    }
}