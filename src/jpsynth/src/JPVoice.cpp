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

void JPVoice::initializeGenerator()
{
    modulationIndex = 0;
    oscmix = 0.0;
    noisemix = 0.0;
    feedbackAmountCarrier = 0.0;
    feedbackAmountModulator = 0.0;

    // create unique bus names
    carrierAudioBusName = "carrierBus" + getName();
    modulatorAudioBusName = "modulatorBus" + getName();
    noiseAudioBusName = "noiseBus" + getName();
    filterCutoffBusName = "filterCutoffBus" + getName();
    outputAmplificationBusName = "outputAmp" + getName();

    // Waveform oscillators
    sawCarrier.initialize("sawCarrier" + getName());
    sawCarrier.setRole(GeneratorRole::Carrier);

    sawModulator.initialize("sawModulator" + getName());
    sawModulator.setRole(GeneratorRole::Normal);

    sineCarrier.initialize("sineCarrier" + getName());
    sineCarrier.setRole(GeneratorRole::Carrier);

    sineModulator.initialize("sineModulator" + getName());
    sineModulator.setRole(GeneratorRole::Normal);

    squareCarrier.initialize("squareCarrier" + getName());
    squareCarrier.setRole(GeneratorRole::Carrier);

    squareModulator.initialize("squareModulator" + getName());
    squareModulator.setRole(GeneratorRole::Normal);

    trianlgeCarrier.initialize("trianlgeCarrier" + getName());
    trianlgeCarrier.setRole(GeneratorRole::Carrier);

    triangleModulator.initialize("triangleModulator" + getName());
    triangleModulator.setRole(GeneratorRole::Normal);

    clusterCarrier.initialize("clusterCarrier" + getName());
    clusterCarrier.setRole(GeneratorRole::Carrier);

    clusterModulator.initialize("clusterModulator" + getName());
    clusterModulator.setRole(GeneratorRole::Normal);

    fibonacciCarrier.initialize("fibonacciCarrier" + getName());
    fibonacciCarrier.setRole(GeneratorRole::Carrier);

    fibonacciModulator.initialize("fibonacciModulator" + getName());
    fibonacciModulator.setRole(GeneratorRole::Normal);

    mirrorCarrier.initialize("mirrorCarrier" + getName());
    mirrorCarrier.setRole(GeneratorRole::Carrier);

    mirrorModulator.initialize("mirrorModulator" + getName());
    mirrorModulator.setRole(GeneratorRole::Normal);

    moduloCarrier.initialize("moduloCarrier" + getName());
    moduloCarrier.setRole(GeneratorRole::Carrier);

    moduloModulator.initialize("moduloModulator" + getName());
    moduloModulator.setRole(GeneratorRole::Normal);

    bitModulator.initialize("bitModulator" + getName());
    bitModulator.setRole(GeneratorRole::Normal);

    filter.initialize("filter" + getName());
    filterAdsr.initialize("filterAdsr" + getName());
    ampAdsr.initialize("ampAdsr" + getName());
    noise.initialize("noise" + getName());
    paramFader.initialize("paramFader" + getName());

    // Create voice exclusive audio and modulation busses
    carrierAudioBus = DSPBusManager::registerAudioBus(carrierAudioBusName);                    // carrier oscillation output bus
    modulatorAudioBus = DSPBusManager::registerAudioBus(modulatorAudioBusName);                // modulator oscillation output bus
    noiseAudioBus = DSPBusManager::registerAudioBus(noiseAudioBusName);                        // noise generator output bus
    filterCutoffBus = DSPBusManager::registerModulationBus(filterCutoffBusName);               // filter cutoff modulation bus (from filterADSR)
    outputAmplificationBus = DSPBusManager::registerModulationBus(outputAmplificationBusName); // output amplification output bus

    // Patching
    carrier->connectOutputToBus(carrierAudioBus);           // carrier output
    carrier->connectFMToBus(modulatorAudioBus);             // FM from modulator
    modulator->connectOutputToBus(modulatorAudioBus);       // modulator output
    noise.connectOutputToBus(noiseAudioBus);                // noise output
    filter.connectModulationToBus(filterCutoffBus);         // cutoff modulation set by filterADSR
    filterAdsr.connectModulationToBus(filterCutoffBus);     // filter adsr on filter cutoff modulation
    ampAdsr.connectModulationToBus(outputAmplificationBus); // voice output amplification

    filterAdsr.setGain(15000.0);
    ampAdsr.setGain(1.0);

    setCarrierOscillatorType(CarrierOscillatiorType::Saw);
    setModulatorOscillatorType(ModulatorOscillatorType::Sine);

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

void JPVoice::onOutputBusConnected(DSPAudioBus &bus)
{
    filter.connectProcessToBus(bus);     // output filtering
    paramFader.connectProcessToBus(bus); // fade output on parameter change
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
    carrierTmp->setAnalogDrift(oscDrift);

    paramFader.change(
        [=]()
        {
            carrier = carrierTmp;

            carrier->connectOutputToBus(carrierAudioBus);
            carrier->connectFMToBus(modulatorAudioBus);

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
    modulatorTmp->setAnalogDrift(oscDrift);

    paramFader.change(
        [=]()
        {
            modulator = modulatorTmp;

            carrier->connectFMToBus(modulatorAudioBus);
            modulator->connectOutputToBus(modulatorAudioBus);

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
    filterCutoff = clamp(f, 0.0, 20000.0);

    filterAdsr.setGain(filterCutoff);
}

// Sets the filter resonance
void JPVoice::setFilterResonance(host_float r)
{
    filterResonance = clampmin(r, 0.0);
    filter.setResonance(filterResonance);
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

void JPVoice::setAnalogDrift(host_float amount)
{
    oscDrift = amount * 0.08;

    carrier->setAnalogDrift(oscDrift);
    modulator->setAnalogDrift(oscDrift);
}

void JPVoice::setFilterCutoffModulationBus(DSPModulationBus &bus)
{
    filterCutoffModulationBus = bus;
}

// Next sample block generation
void JPVoice::processBlock()
{
    modulator->process();

    carrier->process();

    if (syncEnabled && carrier->hasWrapped())
    {
        modulator->resetPhase();
        carrier->unWrap();
    }

    if (noisemix > 0)
    {
        noise.process();
    }

    dsp_math::get_sin_cos(oscmix * 0.5 * dsp_math::DSP_PI, &amp_carrier, &amp_modulator);
    dsp_math::get_sin_cos(oscmix * 0.5 * dsp_math::DSP_PI, &amp_oscs, &amp_noise);

    bool useCarrierFeedback = feedbackAmountCarrier > 0;
    bool useModulatorFeedback = feedbackAmountModulator > 0;
    bool useNoise = noisemix > 0;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        carrierLeft = carrierAudioBus.l[i] + lastSampleCarrierLeft * feedbackAmountCarrier;
        carrierRight = carrierAudioBus.r[i] + lastSampleCarrierRight * feedbackAmountCarrier;
        modLeft = modulatorAudioBus.l[i] + lastSampleModulatorLeft * feedbackAmountModulator;
        modRight = modulatorAudioBus.r[i] + lastSampleModulatorRight * feedbackAmountModulator;

        mixL = amp_carrier * carrierLeft + amp_modulator * modLeft;
        mixR = amp_carrier * carrierRight + amp_modulator * modRight;

        if (useCarrierFeedback)
        {
            lastSampleCarrierLeft = tanh(carrierLeft); 
            lastSampleCarrierRight = tanh(carrierRight);
        }

        if (useModulatorFeedback)
        {
            lastSampleModulatorLeft = tanh(modLeft);
            lastSampleModulatorRight = tanh(modRight);
        }

        if (useNoise)
        {
            mixL = amp_oscs * mixL + amp_noise * noiseAudioBus.l[i];
            mixR = amp_oscs * mixR + amp_noise * noiseAudioBus.r[i];
        }

        outputBus.l[i] = mixL;
        outputBus.r[i] = mixR;
    }

    // Filter cutoff calculation
    filterAdsr.process();

    // Compute LFO modulation on cutoff
    filterCutoffBus.multiplyWidth(filterCutoffModulationBus);

    // process filter
    filter.process();

    // output amplification
    ampAdsr.processMultiply(outputBus);

    // Assign changed params
    paramFader.process();
}

// Next sample block generation
void JPVoice::processBlock(DSPObject *dsp)
{
    JPVoice *self = static_cast<JPVoice *>(dsp);

    self->processBlock();
}