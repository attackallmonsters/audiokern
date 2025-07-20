#include "JPVoice.h"

// Constructor: initializes the voice with two oscillator instances.
// These oscillators are externally allocated and represent the carrier (carrier) and modulator (modulator).
JPVoice::JPVoice()
{
    carrier = &sawCarrier;
    modulator = &sineModulator;

    registerBlockProcessor(JPVoice::processBlock);
}

// Destructor: cleans up oscillator instances.
// It is assumed that carrier and modulator were allocated using 'new' and are not shared elsewhere.
JPVoice::~JPVoice()
{
}

void JPVoice::initialize()
{
    DSPObject::initialize();

    modulationIndex = 0;
    oscmix = 0.0;
    noisemix = 0.0;
    feedbackAmountCarrier = 0.0;
    feedbackAmountModulator = 0.0;

    noise.initialize();

    // Waveform generation
    sawCarrier.initialize();
    sawModulator.initialize();
    sineCarrier.initialize();
    sineModulator.initialize();
    squareCarrier.initialize();
    squareModulator.initialize();
    trianlgeCarrier.initialize();
    triangleModulator.initialize();
    clusterCarrier.initialize();
    clusterModulator.initialize();
    fibonacciCarrier.initialize();
    fibonacciModulator.initialize();
    mirrorCarrier.initialize();
    mirrorModulator.initialize();
    moduloCarrier.initialize();
    moduloModulator.initialize();
    bitModulator.initialize();

    filter.initialize();

    filterAdsr.initialize();
    ampAdsr.initialize();

    filterAdsr.setGain(15000.0);
    ampAdsr.setGain(1.0);

    resoBuffer.create(DSP::blockSize);
    resoBuffer.fill(0.0);

    // Real buffers will be set to consumers output buffers
    outputBufferL.initialize(DSP::blockSize);
    outputBufferR.initialize(DSP::blockSize);

    setCarrierOscillatorType(CarrierOscillatiorType::Saw);
    setModulatorOscillatorType(ModulatorOscillatorType::Sine);

    carrier->modulationBufferL = modulator->outputBufferL;
    carrier->modulationBufferR = modulator->outputBufferR;

    linkADSR(true);

    ADSRParams p;
    p.attackTime = 10.0;
    p.decayTime = 0.0;
    p.sustainLevel = 1.0;
    p.releaseTime = 750.0;
    p.attackShape = 0.0;
    p.releaseShape = 0.0;

    setFilterADSR(p);
    setAmpADSR(p);

    setCarrierFrequency(0.0);
    setModulatorFrequency(0.0);
    setDetune(0.0);
    setSyncEnabled(false);
    setNumVoices(1);
    setModIndex(0.0);
}

// Start ADSRs
void JPVoice::playNote()
{
    filterAdsr.triggerStart();
    ampAdsr.triggerStart();
}

// Stop  ADSRs
void JPVoice::stopNote()
{
    filterAdsr.triggerStop();
    ampAdsr.triggerStop();
}

// Sets the modulation index for frequency modulation.
// This controls the intensity of the frequency modulation effect.
void JPVoice::setModIndex(host_float index)
{
    modulationIndex = index;
    carrier->setModIndex(modulationIndex);
}

// Enables or disables oscillator synchronization.
// When enabled and carrier wraps its phase, modulator will be reset to phase 0.
void JPVoice::setSyncEnabled(bool enabled)
{
    syncEnabled = enabled;
}

// Sets the current frequency for the carrier
void JPVoice::setCarrierFrequency(host_float f)
{
    carrier->setFrequency(f);
    carrierFrequency = f;
}

// Sets the current frequency for the modulator
void JPVoice::setModulatorFrequency(host_float f)
{
    modulator->setFrequency(f);
    modulatorFrequency = f;
}

// Sets the detune factorjpvoice_tilde_sync
void JPVoice::setDetune(host_float value)
{
    detune = value;
    carrier->setDetune(detune);
}

// Sets the number of voices
void JPVoice::setNumVoices(int count)
{
    if (count == numVoices)
        return;

    numVoices = count;
    paramFader.change(
        [=]()
        {
            carrier->setNumVoices(numVoices);
        });
}

// Sets the volume level of the oscillators
void JPVoice::setOscillatorMix(host_float mix)
{
    oscmix = clamp(mix, 0.0, 1.0);
}

// Sets the volume level of the noise generator
void JPVoice::setNoiseMix(host_float mix)
{
    noisemix = clamp(mix, 0.0, 1.0);
}

// Assigns the carrier oscillator
void JPVoice::setCarrierOscillatorType(CarrierOscillatiorType oscillatorType)
{
    switch (oscillatorType)
    {
    case CarrierOscillatiorType::Saw:
        carrierTmp = &sawCarrier;
        break;
    case CarrierOscillatiorType::Square:
        carrierTmp = &squareCarrier;
        break;
    case CarrierOscillatiorType::Triangle:
        carrierTmp = &trianlgeCarrier;
        break;
    case CarrierOscillatiorType::Sine:
        carrierTmp = &sineCarrier;
        break;
    case CarrierOscillatiorType::Cluster:
        carrierTmp = &clusterCarrier;
        break;
    case CarrierOscillatiorType::Fibonacci:
        carrierTmp = &fibonacciCarrier;
        break;
    case CarrierOscillatiorType::Mirror:
        carrierTmp = &mirrorCarrier;
        break;
    case CarrierOscillatiorType::Modulo:
        carrierTmp = &moduloCarrier;
        break;
    default:
        carrierTmp = &sawCarrier;
        break;
    }

    if (carrierTmp == carrier)
    {
        return;
    }

    carrierTmp->setFrequency(carrierFrequency);
    carrierTmp->setModIndex(modulationIndex);
    carrierTmp->setDetune(detune);
    carrierTmp->setNumVoices(numVoices);

    paramFader.change(
        [=]()
        {
            carrier = carrierTmp;
            carrier->modulationBufferL = modulator->outputBufferL;
            carrier->modulationBufferR = modulator->outputBufferR;

            filter.reset();
        });
}

// Assigns the modulation oscillator
void JPVoice::setModulatorOscillatorType(ModulatorOscillatorType oscillatorType)
{
    switch (oscillatorType)
    {
    case ModulatorOscillatorType::Saw:
        modulatorTmp = &sawModulator;
        break;
    case ModulatorOscillatorType::Square:
        modulatorTmp = &squareModulator;
        break;
    case ModulatorOscillatorType::Triangle:
        modulatorTmp = &triangleModulator;
        break;
    case ModulatorOscillatorType::Sine:
        modulatorTmp = &sineModulator;
        break;
    case ModulatorOscillatorType::Cluster:
        modulatorTmp = &clusterModulator;
        break;
    case ModulatorOscillatorType::Fibonacci:
        modulatorTmp = &fibonacciModulator;
        break;
    case ModulatorOscillatorType::Mirror:
        modulatorTmp = &mirrorModulator;
        break;
    case ModulatorOscillatorType::Modulo:
        modulatorTmp = &moduloModulator;
        break;
    case ModulatorOscillatorType::Bit:
        modulatorTmp = &bitModulator;
        break;
    default:
        modulatorTmp = &sineModulator;
        break;
    }

    if (modulatorTmp == modulator)
    {
        return;
    }

    modulatorTmp->setFrequency(modulatorFrequency);

    paramFader.change(
        [=]()
        {
            modulator = modulatorTmp;
            carrier->modulationBufferL = modulator->outputBufferL;
            carrier->modulationBufferR = modulator->outputBufferR;

            filter.reset();
        });
}

// Changes the current noise type (white or pink)
void JPVoice::setNoiseType(NoiseType type)
{
    noise.setType(type);
}

// Sets the feedback amount for the carrier
void JPVoice::setFeedbackCarrier(host_float feedback)
{
    feedbackAmountCarrier = clamp(feedback, 0.0, 2.0);
}

// Sets the feedback amount for the modulator
void JPVoice::setFeedbackModulator(host_float feedback)
{
    feedbackAmountModulator = clamp(feedback, 0.0, 2.0);
}

// Sets the filter type
void JPVoice::setFilterMode(FilterMode mode)
{
    if (mode == FilterMode::LP)
        setFilterCutoff(15000.0);
    else
        setFilterCutoff(0.0);

    filter.setFilterMode(mode);
}

// Sets the cutoff frequency
void JPVoice::setFilterCutoff(host_float f)
{
    filterAdsr.setGain(clamp(f, 0.0, 20000.0));
}

// Sets the filter resonance
void JPVoice::setFilterResonance(host_float r)
{
    filterResonance = clampmin(r, 0.0);
}

// Sets the filter drive
void JPVoice::setFilterDrive(host_float value)
{
    filter.setDrive(value);
}

// Filter envelope params
void JPVoice::setFilterADSR(ADSRParams &params)
{
    setFilterADSRLink(params, adsrLinked);
}

// Amplification envelope params
void JPVoice::setAmpADSR(ADSRParams &params)
{
    setAmpADSRLink(params, adsrLinked);
}

void JPVoice::setFilterADSRLink(ADSRParams &params, bool setOther)
{
    filterAdsr.setAttack(params.attackTime);
    filterAdsr.setDecay(params.decayTime);
    filterAdsr.setSustain(params.sustainLevel);
    filterAdsr.setRelease(params.releaseTime);
    filterAdsr.setAttackShape(params.attackShape);
    filterAdsr.setReleaseShape(params.releaseShape);

    if (setOther)
    {
        setAmpADSRLink(params, false);
    }
}

void JPVoice::setAmpADSRLink(ADSRParams &params, bool setOther)
{
    ampAdsr.setAttack(params.attackTime);
    ampAdsr.setDecay(params.decayTime);
    ampAdsr.setSustain(params.sustainLevel);
    ampAdsr.setRelease(params.releaseTime);
    ampAdsr.setAttackShape(params.attackShape);
    ampAdsr.setReleaseShape(params.releaseShape);

    if (setOther)
    {
        setFilterADSRLink(params, false);
    }
}

// Link the ADSRs
void JPVoice::linkADSR(bool isEnabled)
{
    adsrLinked = isEnabled;
}
// Set adrss to one shot mode
void JPVoice::setADSROneshot(bool isEnabled)
{
    filterAdsr.setOneShot(isEnabled);
    ampAdsr.setOneShot(isEnabled);
}

void JPVoice::setAmpGain(host_float g)
{
    ampAdsr.setGain(clampmin(g, 0.0));
}

// Sets the output buffers
void JPVoice::setOutputBuffer(DSPSampleBuffer &bufL, DSPSampleBuffer &bufR)
{
    outputBufferL = bufL;
    outputBufferR = bufR;

    paramFader.outputBufferL = outputBufferL;
    paramFader.outputBufferR = outputBufferR;

    filter.processBufferL = outputBufferL;
    filter.processBufferR = outputBufferR;
    filter.cutoffBuffer = filterAdsr.outputBuffer;
    filter.resoBuffer = resoBuffer;
}

// Next sample block generation
void JPVoice::processBlock()
{
    modulator->generateBlock();

    carrier->generateBlock();

    if (syncEnabled && carrier->hasWrapped())
    {
        modulator->resetPhase();
        carrier->unWrap();
    }

    if (noisemix > 0)
    {
        noise.generateBlock();
    }

    amp_carrier = std::cos(oscmix * 0.5 * M_PI);
    amp_modulator = std::sin(oscmix * 0.5 * M_PI);
    amp_osc_noise = std::cos(noisemix * 0.5 * M_PI);
    amp_noise = std::sin(noisemix * 0.5 * M_PI);

    resoBuffer.fill(filterResonance);

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        carrierLeft = carrier->outputBufferL[i] + lastSampleCarrierLeft * feedbackAmountCarrier;
        carrierRight = carrier->outputBufferR[i] + lastSampleCarrierRight * feedbackAmountCarrier;
        modLeft = modulator->outputBufferL[i] + lastSampleModulatorLeft * feedbackAmountModulator;
        modRight = modulator->outputBufferR[i] + lastSampleModulatorRight * feedbackAmountModulator;

        mixL = amp_carrier * carrierLeft + amp_modulator * modLeft;
        mixR = amp_carrier * carrierRight + amp_modulator * modRight;

        if (feedbackAmountCarrier > 0)
        {
            lastSampleCarrierLeft = dsp_math::fast_tanh(carrierLeft);
            lastSampleCarrierRight = dsp_math::fast_tanh(carrierRight);
        }

        if (feedbackAmountModulator > 0)
        {
            lastSampleModulatorLeft = dsp_math::fast_tanh(modLeft);
            lastSampleModulatorRight = dsp_math::fast_tanh(modRight);
        }

        if (noisemix > 0)
        {
            mixL = amp_osc_noise * mixL + amp_noise * noise.outputBufferL[i];
            mixR = amp_osc_noise * mixR + amp_noise * noise.outputBufferR[i];
        }

        outputBufferL[i] = mixL;
        outputBufferR[i] = mixR;
    }

    // ADSR shares buffer with filter
    filterAdsr.generateBlock();
    filter.generateBlock();

    // amp envelope
    ampAdsr.generateBlock();
    ampAdsr.multiply(outputBufferL, outputBufferR);

    // Assign changed params
    paramFader.generateBlock();
}

// Next sample block generation
void JPVoice::processBlock(DSPObject *dsp)
{
    JPVoice* self = static_cast<JPVoice *>(dsp);

    self->processBlock();
}