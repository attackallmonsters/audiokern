#include "JPVoice.h"

// Constructor: initializes the voice with two oscillator instances.
// These oscillators are externally allocated and represent the carrier (carrier) and modulator (modulator).
JPVoice::JPVoice()
{
    carrier = sawCarrier;
    modulator = sineModulator;
}

// Destructor: cleans up oscillator instances.
// It is assumed that carrier and modulator were allocated using 'new' and are not shared elsewhere.
JPVoice::~JPVoice()
{
    delete noise;
    delete sineCarrier;
    delete sineModulator;
    delete sawCarrier;
    delete sawModulator;
    delete squareCarrier;
    delete squareModulator;
    delete trianlgeCarrier;
    delete triangleModulator;
    delete clusterCarrier;
    delete clusterModulator;
    delete fibonacciCarrier;
    delete fibonacciModulator;
    delete mirrorCarrier;
    delete mirrorModulator;
    delete moduloCarrier;
    delete moduloModulator;
    delete bitModulator;
}

void JPVoice::initialize()
{
    if (!DSP::isInitialized())
    {
        DSP::log("DSP not initialized. Do DSP::initializeAudio first.");
        throw("DSP not initialized. Do DSP::initializeAudio first.");
    }

    DSP::log("=====> Initializing jpvoice...");

    DSPObject::initialize();

    modulationIndex = 0;
    oscmix = 0.0;
    noisemix = 0.0;
    feedbackAmountCarrier = 0.0;
    feedbackAmountModulator = 0.0;

    noise->initialize();

    // Waveform generation
    sawCarrier->initialize();
    sawModulator->initialize();
    sineCarrier->initialize();
    sineModulator->initialize();
    squareCarrier->initialize();
    squareModulator->initialize();
    trianlgeCarrier->initialize();
    triangleModulator->initialize();
    clusterCarrier->initialize();
    clusterModulator->initialize();
    fibonacciCarrier->initialize();
    fibonacciModulator->initialize();
    mirrorCarrier->initialize();
    mirrorModulator->initialize();
    moduloCarrier->initialize();
    moduloModulator->initialize();
    bitModulator->initialize();

    filter->initialize();

    mixBufferL.resize(DSP::blockSize);
    mixBufferR.resize(DSP::blockSize);

    setCarrierOscillatorType(CarrierOscillatiorType::Saw);
    setModulatorOscillatorType(ModulatorOscillatorType::Sine);

    setFrequency(0.0);
    setDetune(0.0);
    setSyncEnabled(false);
    setPitchOffset(0.0);
    setFineTune(0.0);
    setNumVoices(1);
    setModIndex(0.0);

    DSP::log("=====> jpvoice initialized");
}

// Sets the modulation index for frequency modulation.
// This controls the intensity of the frequency modulation effect.
void JPVoice::setModIndex(dsp_float index)
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

// Sets the pitch offset for the modulator
void JPVoice::setPitchOffset(int offset)
{
    pitchOffset = offset;
    modulator->setPitchOffset(pitchOffset);
}

// Sets the fine tunig for the modulator
void JPVoice::setFineTune(dsp_float fine)
{
    fineTune = fine;
    modulator->setFineTune(fineTune);
}

// Sets the current frequency
void JPVoice::setFrequency(dsp_float f)
{
    carrier->setFrequency(f);
    modulator->setFrequency(f);
    frequency = f;
}

// Sets the detune factorjpvoice_tilde_sync
void JPVoice::setDetune(dsp_float value)
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
    paramFader.change([=]()
                      { carrier->setNumVoices(numVoices); });
}

// Sets the volume level of the oscillators
void JPVoice::setOscillatorMix(dsp_float mix)
{
    oscmix = clamp(mix, 0.0, 1.0);
}

// Sets the volume level of the noise generator
void JPVoice::setNoiseMix(dsp_float mix)
{
    noisemix = clamp(mix, 0.0, 1.0);
}

// Assigns the carrier oscillator
void JPVoice::setCarrierOscillatorType(CarrierOscillatiorType oscillatorType)
{
    dsp_float f = (carrier) ? carrier->getFrequency() : 0.0;

    switch (oscillatorType)
    {
    case CarrierOscillatiorType::Saw:
        carrierTmp = sawCarrier;
        break;
    case CarrierOscillatiorType::Square:
        carrierTmp = squareCarrier;
        break;
    case CarrierOscillatiorType::Triangle:
        carrierTmp = trianlgeCarrier;
        break;
    case CarrierOscillatiorType::Sine:
        carrierTmp = sineCarrier;
        break;
    case CarrierOscillatiorType::Cluster:
        carrierTmp = clusterCarrier;
        break;
    case CarrierOscillatiorType::Fibonacci:
        carrierTmp = fibonacciCarrier;
        break;
    case CarrierOscillatiorType::Mirror:
        carrierTmp = mirrorCarrier;
        break;
    case CarrierOscillatiorType::Modulo:
        carrierTmp = moduloCarrier;
        break;
    default:
        carrierTmp = sawCarrier;
        break;
    }

    if (carrierTmp == carrier)
    {
        return;
    }

    carrierTmp->setFrequency(f);
    carrierTmp->setModIndex(modulationIndex);
    carrierTmp->setDetune(detune);
    carrierTmp->setNumVoices(numVoices);

    paramFader.change([=]()
                      {
         if (carrier != carrierTmp)
            carrier = carrierTmp;

         if (modulator != modulatorTmp)
            modulator = modulatorTmp;

         filter->reset(); });
}

// Assigns the modulation oscillator
void JPVoice::setModulatorOscillatorType(ModulatorOscillatorType oscillatorType)
{
    switch (oscillatorType)
    {
    case ModulatorOscillatorType::Saw:
        modulatorTmp = sawModulator;
        break;
    case ModulatorOscillatorType::Square:
        modulatorTmp = squareModulator;
        break;
    case ModulatorOscillatorType::Triangle:
        modulatorTmp = triangleModulator;
        break;
    case ModulatorOscillatorType::Sine:
        modulatorTmp = sineModulator;
        break;
    case ModulatorOscillatorType::Cluster:
        modulatorTmp = clusterModulator;
        break;
    case ModulatorOscillatorType::Fibonacci:
        modulatorTmp = fibonacciModulator;
        break;
    case ModulatorOscillatorType::Mirror:
        modulatorTmp = mirrorModulator;
        break;
    case ModulatorOscillatorType::Modulo:
        modulatorTmp = moduloModulator;
        break;
    case ModulatorOscillatorType::Bit:
        modulatorTmp = bitModulator;
        break;
    default:
        modulatorTmp = sineModulator;
        break;
    }

    if (modulatorTmp == modulator)
    {
        return;
    }

    modulatorTmp->setFrequency(frequency);
    modulatorTmp->setPitchOffset(pitchOffset);
    modulatorTmp->setFineTune(fineTune);

    paramFader.change([=]()
                      {
         if (carrier != carrierTmp)
            carrier = carrierTmp;

         if (modulator != modulatorTmp)
            modulator = modulatorTmp;

         filter->reset(); });
}

// Changes the current noise type (white or pink)
void JPVoice::setNoiseType(NoiseType type)
{
    noise->setType(type);
}

// Sets the feedback amount for the carrier
void JPVoice::setFeedbackCarrier(dsp_float feedback)
{
    feedbackAmountCarrier = clamp(feedback, 0.0, 2.0);
}

// Sets the feedback amount for the modulator
void JPVoice::setFeedbackModulator(dsp_float feedback)
{
    feedbackAmountModulator = clamp(feedback, 0.0, 2.0);
}

// Sets the filter type
void JPVoice::setFilterMode(FilterMode /*mode*/)
{
    // TODO
}

// Sets the cutoff frequency
void JPVoice::setFilterCutoff(DSPBuffer *buffer)
{
    filter->setCutoff(buffer);
}

// Sets the filter resonance
void JPVoice::setFilterResonance(DSPBuffer *buffer)
{
    filter->setResonance(buffer);
}

// Sets the filter drive
void JPVoice::setFilterDrive(dsp_float value)
{
    filter->setDrive(value);
}

// Next sample block generation
void JPVoice::computeSamples()
{
    modulator->generateBlock();

    carrier->modBufferL.switchTo(modulator->outBufferL);
    carrier->modBufferR.switchTo(modulator->outBufferR);

    carrier->generateBlock();

    if (syncEnabled && carrier->hasWrapped())
    {
        modulator->resetPhase();
        carrier->unWrap();
    }

    if (noisemix > 0)
    {
        noise->generateBlock();
    }

    amp_carrier = std::cos(oscmix * 0.5 * M_PI);
    amp_modulator = std::sin(oscmix * 0.5 * M_PI);
    amp_osc_noise = std::cos(noisemix * 0.5 * M_PI);
    amp_noise = std::sin(noisemix * 0.5 * M_PI);

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        carrierLeft = carrier->outBufferL[i] + lastSampleCarrierLeft * feedbackAmountCarrier;
        carrierRight = carrier->outBufferR[i] + lastSampleCarrierRight * feedbackAmountCarrier;
        modLeft = modulator->outBufferL[i] + lastSampleModulatorLeft * feedbackAmountModulator;
        modRight = modulator->outBufferR[i] + lastSampleModulatorRight * feedbackAmountModulator;

        mixL = amp_carrier * carrierLeft + amp_modulator * modLeft;
        mixR = amp_carrier * carrierRight + amp_modulator * modRight;

        if (feedbackAmountCarrier > 0)
        {
            lastSampleCarrierLeft = fast_tanh(carrierLeft);
            lastSampleCarrierRight = fast_tanh(carrierRight);
        }

        if (feedbackAmountModulator > 0)
        {
            lastSampleModulatorLeft = fast_tanh(modLeft);
            lastSampleModulatorRight = fast_tanh(modRight);
        }

        if (noisemix > 0)
        {
            mixL = amp_osc_noise * mixL + amp_noise * noise->outBufferL[i];
            mixR = amp_osc_noise * mixR + amp_noise * noise->outBufferR[i];
        }

        mixBufferL[i] = mixL;
        mixBufferR[i] = mixR;
    }

    filter->setSampleBuffers(&mixBufferL, &mixBufferR);

    filter->generateBlock();

    paramFader.processChanges(mixBufferL, mixBufferR);
}
