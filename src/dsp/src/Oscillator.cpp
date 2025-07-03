#include "Oscillator.h"


// no FM
static dsp_float fmNone(const dsp_float &base, const dsp_float & /*mod*/, const dsp_float & /*index*/)
{
    return base;
}

// Linear FM
static dsp_float fmLinear(const dsp_float &base, const dsp_float &mod, const dsp_float &index)
{
    return base + mod * index;
}

// Relative FM
static dsp_float fmRelative(const dsp_float &base, const dsp_float &mod, const dsp_float &index)
{
    return base + mod * index * base;
}

// Trough Zero FM
static dsp_float fmThroughZero(const dsp_float &base, const dsp_float &mod, const dsp_float &index)
{
    return base + mod * index;
}

Oscillator::Oscillator()
{
    // to avoid vtable lookup
    registerBlockProcessor(&Oscillator::processBlock);
    generateSampleFunc = &Oscillator::generateSample;
    fmFunc = fmThroughZero;
}

Oscillator::~Oscillator()
{
}

// Initializes the oscillator
void Oscillator::initialize()
{
    DSPObject::initialize();

    outBufferL.resize(DSP::blockSize);
    outBufferR.resize(DSP::blockSize);
    modBufferL.resize(DSP::blockSize);
    modBufferR.resize(DSP::blockSize);

    setFrequency(0.0);
    setFineTune(0);
    setPitchOffset(0);
    setNumVoices(1);
    setNegativeWrappingEnabled(false);
    setFMType(FMType::ThroughZero);
    setModIndex(0.0);
    unWrap();
}

// Derived classes registers sample generator
void Oscillator::registerSampleGenerator(SampleGenerator sg)
{
    generateSampleFunc = sg;
}

// Resets the internal oscillator phase to 0.0.
void Oscillator::resetPhase()
{
    currentPhase = 0.0;
    wrapped = false;
}

// Sets the desired oscillator frequency in Hertz
void Oscillator::setFrequency(dsp_float value)
{
    if (value == frequency)
        return;

    frequency = clampmin(value, 0.0);
    setCalculatedFrequency(frequency);
}

// Sets the pitch offset in semi tones
void Oscillator::setPitchOffset(int value)
{
    pitchOffset = clamp(value, -24, 24);
    setCalculatedFrequency(frequency);
}

// Sets the fine tuning in cent
void Oscillator::setFineTune(dsp_float value)
{
    fineTune = clamp(value, -100.0, 100.0);
    setCalculatedFrequency(frequency);
}

// Enables negative phase wrapping
void Oscillator::setNegativeWrappingEnabled(bool enabled)
{
    negativeWrappingEnabled = enabled;
}

// Calculates the effective frequency based on base frequency,
// pitch offset (in semitones), and fine-tuning (in cents).
// Then updates the phase increment accordingly.
void Oscillator::setCalculatedFrequency(dsp_float f)
{
    // Convert pitch offset and fine tune to a total semitone offset
    dsp_float semitoneOffset = static_cast<dsp_float>(pitchOffset) + (fineTune / 100.0);

    // Apply equal temperament formula: f = f0 * 2^(n/12)
    calculatedFrequency = f * std::pow(2.0, semitoneOffset / 12.0);

    // Update phase increment for waveform generation
    phaseIncrement = calculatedFrequency / DSP::sampleRate;
}

// Gets the current frequency
dsp_float Oscillator::getFrequency()
{
    return frequency;
}

// Gets the calculated FM frequency
dsp_float Oscillator::getCalculatedFrequency()
{
    return calculatedFrequency;
}

// Returns true if the oscillator's phase wrapped during the last getSample() call
bool Oscillator::hasWrapped()
{
    return wrapped;
}

// Resets the wrap status
void Oscillator::unWrap()
{
    wrapped = false;
}

// Sets the type of FM to use
void Oscillator::setFMType(FMType fm)
{
    fmType = fm;
    negativeWrappingEnabled = fmType == FMType::ThroughZero;
    setNegativeWrappingEnabled(negativeWrappingEnabled);

    if (fmType == FMType::Relative && modulationIndex > 30.0)
    {
        setModIndex(30);
    }

    switch (fmType)
    {
    case FMType::None:
        fmFunc = fmNone;
        break;
    case FMType::Linear:
        fmFunc = fmLinear;
        break;
    case FMType::Relative:
        fmFunc = fmRelative;
        break;
    case FMType::ThroughZero:
        fmFunc = fmThroughZero;
        break;
    default:
        fmFunc = fmNone;
        break;
    }
}

// Sets the modulation index for frequency modulation.
// This controls the intensity of the frequency modulation effect.
void Oscillator::setModIndex(dsp_float index)
{
    dsp_float modmax = (fmType == FMType::Relative) ? 30 : 1000;
    modulationIndex = clamp(index, 0.0, modmax);

    if (modulationIndex == 0)
    {
        setFrequency(frequency);
        return;
    }
}

// Dummy ComputeSampleFunc for setSamples
void Oscillator::generateSample(Oscillator *,
                                const dsp_float & /*frequency*/,
                                const dsp_float &phase,
                                dsp_float &left,
                                dsp_float &right,
                                const dsp_float & /*modLeft*/,
                                const dsp_float & /*modRight*/)
{
    left = right = std::sin(phase * 2.0 * M_PI); // Sine generator
}

// Next sample block generation
void Oscillator::processBlock(DSPObject *dsp)
{
    Oscillator *osc = static_cast<Oscillator *>(dsp);

    dsp_float phase = osc->currentPhase;
    bool wrappedFlag = false;
    dsp_float baseFreq = osc->calculatedFrequency;
    dsp_float index = osc->modulationIndex;
    FMType fmType = osc->fmType;
    dsp_float sr = DSP::sampleRate;
    dsp_float phaseIncrement = osc->phaseIncrement;
    dsp_float left, right;
    bool negativeWrappingEnabled = osc->negativeWrappingEnabled;
    size_t blocksize = DSP::blockSize;

    if (index > 0.0 && fmType != FMType::None)
    {
        for (size_t i = 0; i < blocksize; ++i)
        {
            dsp_float modLeft = osc->modBufferL[i];
            dsp_float modRight = osc->modBufferR[i];

            dsp_float mod = 0.5 * (modLeft + modLeft);
            dsp_float freq = osc->fmFunc(baseFreq, mod, index);
            dsp_float inc = freq / sr;

            phase += inc;

            if (phase >= 1.0)
            {
                phase -= 1.0;
                wrappedFlag = true;
            }
            else if (phase < 0.0 && negativeWrappingEnabled)
            {
                phase += 1.0;
                wrappedFlag = true;
            }

            osc->generateSampleFunc(osc, freq, phase, left, right, modLeft, modRight);
            osc->outBufferL[i] = left;
            osc->outBufferR[i] = right;
        }
    }
    else
    {
        for (size_t i = 0; i < blocksize; ++i)
        {
            dsp_float modLeft = osc->modBufferL[i];
            dsp_float modRight = osc->modBufferR[i];

            // TODO: sometimes becomes huge
            phase += phaseIncrement;

            if (phase >= 1.0)
            {
                phase -= 1.0;
                wrappedFlag = true;
            }

            osc->generateSampleFunc(osc, baseFreq, phase, left, right, modLeft, modRight);
            osc->outBufferL[i] = left;
            osc->outBufferR[i] = right;
        }
    }

    osc->currentPhase = phase;
    osc->wrapped = wrappedFlag;
}
