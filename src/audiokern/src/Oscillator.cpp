#include "Oscillator.h"


// no FM
static host_float fmNone(const host_float &base, const host_float & /*mod*/, const host_float & /*index*/)
{
    return base;
}

// Linear FM
static host_float fmLinear(const host_float &base, const host_float &mod, const host_float &index)
{
    return base + mod * index;
}

// Relative FM
static host_float fmRelative(const host_float &base, const host_float &mod, const host_float &index)
{
    return base + mod * index * base;
}

// Trough Zero FM
static host_float fmThroughZero(const host_float &base, const host_float &mod, const host_float &index)
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
DSPUsage Oscillator::initializeObject()
{
    setFrequency(0.0);
    setFineTune(0);
    setPitchOffset(0);
    setNumVoices(1);
    setNegativeWrappingEnabled(false);
    setFMType(FMType::ThroughZero);
    setModIndex(0.0);
    unWrap();

    return DSPUsage::FM;
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
void Oscillator::setFrequency(host_float value)
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
void Oscillator::setFineTune(host_float value)
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
void Oscillator::setCalculatedFrequency(host_float f)
{
    // Convert pitch offset and fine tune to a total semitone offset
    host_float semitoneOffset = static_cast<host_float>(pitchOffset) + (fineTune / 100.0);

    // Apply equal temperament formula: f = f0 * 2^(n/12)
    calculatedFrequency = f * std::pow(2.0, semitoneOffset / 12.0);

    // Update phase increment for waveform generation
    phaseIncrement = calculatedFrequency / DSP::sampleRate;
}

// Gets the current frequency
host_float Oscillator::getFrequency()
{
    return frequency;
}

// Gets the calculated FM frequency
host_float Oscillator::getCalculatedFrequency()
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
void Oscillator::setModIndex(host_float index)
{
    host_float modmax = (fmType == FMType::Relative) ? 30 : 1000;
    modulationIndex = clamp(index, 0.0, modmax);

    if (modulationIndex == 0)
    {
        setFrequency(frequency);
        return;
    }
}

// Dummy ComputeSampleFunc for setSamples
void Oscillator::generateSample(Oscillator *,
                                const host_float & /*frequency*/,
                                const host_float &phase,
                                host_float &left,
                                host_float &right,
                                const host_float & /*modLeft*/,
                                const host_float & /*modRight*/)
{
    left = right = std::sin(phase * 2.0 * dsp_math::DSP_PI); // Sine generator
}

// Next sample block generation
void Oscillator::processBlock(DSPObject *dsp)
{
    Oscillator *osc = static_cast<Oscillator *>(dsp);

    host_float phase = osc->currentPhase;
    bool wrappedFlag = false;
    host_float baseFreq = osc->calculatedFrequency;
    host_float index = osc->modulationIndex;
    FMType fmType = osc->fmType;
    host_float sr = DSP::sampleRate;
    host_float phaseIncrement = osc->phaseIncrement;
    host_float left, right;
    bool negativeWrappingEnabled = osc->negativeWrappingEnabled;
    size_t blocksize = DSP::blockSize;

    if (index > 0.0 && fmType != FMType::None)
    {
        for (size_t i = 0; i < blocksize; ++i)
        {
            host_float modLeft = osc->fmBufferL[i];
            host_float modRight = osc->fmBufferR[i];

            host_float mod = 0.5 * (modLeft + modLeft);
            host_float freq = osc->fmFunc(baseFreq, mod, index);
            host_float inc = freq / sr;

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
            osc->outputBufferL[i] = left;
            osc->outputBufferR[i] = right;
        }
    }
    else
    {
        for (size_t i = 0; i < blocksize; ++i)
        {
            // TODO: sometimes becomes huge
            phase += phaseIncrement;

            if (phase >= 1.0)
            {
                phase -= 1.0;
                wrappedFlag = true;
            }

            osc->generateSampleFunc(osc, baseFreq, phase, left, right, 0.0, 0.0);
            osc->outputBufferL[i] = left;
            osc->outputBufferR[i] = right;
        }
    }

    osc->currentPhase = phase;
    osc->wrapped = wrappedFlag;
}
