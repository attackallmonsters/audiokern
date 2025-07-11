#include "JPSynth.h"

JPSynth &JPSynth::instance()
{
    static JPSynth instance; // Wird beim ersten Aufruf erstellt
    return instance;
}

void JPSynth::initialize(host_float *bufL, host_float *bufR)
{
    if (!DSP::isInitialized())
    {
        DSP::log("DSP not initialized. Do DSP::initializeAudio first.");
        throw("DSP not initialized. Do DSP::initializeAudio first.");
    }

    DSP::log("=====> Initializing jpvoice...");

    carrierTuning.initialize();
    modulatorTuning.initialize();
    midi.initialize();

    createVoices();

    mixerL.clear();
    mixerR.clear();

    mixerL.create(voiceCount, bufL);
    mixerR.create(voiceCount, bufR);

    for (size_t i = 0; i < voiceCount; ++i)
    {
        SynthVoice *voice = allocator.getVoice(i);

        voice->jpvoice.setOutputBuffer(mixerL.getBuffer(i), mixerR.getBuffer(i));
    }

    DSP::log("=====> jpvoice initialized");
}

void JPSynth::noteIn(int note, host_float velocity)
{
    if (velocity > 0)
    {
        currentVoice = allocator.allocate(note);
        currentVoice->note = note;
        currentVoice->jpvoice.setCarrierFrequency(carrierTuning.frequency(note));
        currentVoice->jpvoice.setModulatorFrequency(modulatorTuning.frequency(note));
        currentVoice->jpvoice.setAmpGain(midi.normalizeVelocityRMS(velocity));
        currentVoice->jpvoice.playNote();
    }
    else
    {
        currentVoice = allocator.select(note);

        // May have already been reallocated
        if (!currentVoice)
            return;

        currentVoice->jpvoice.stopNote();
        allocator.setReclaimable(note);
    }
}

void JPSynth::setOscillatorMix(host_float mix)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setOscillatorMix(mix);
        });
}

void JPSynth::setNoiseMix(host_float mix)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setNoiseMix(mix);
        });
}

void JPSynth::setPitchOffset(host_float offset)
{
    int val = clamp(offset, -24, 24);

    modulatorTuning.setHalftoneOffset(val);

    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setModulatorFrequency(modulatorTuning.frequency(v.note));
        });
}

void JPSynth::setFineTune(host_float fine)
{
    int val = clamp(fine, -2400.0, 2400.0);

    modulatorTuning.setFinetune(val);

    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setModulatorFrequency(modulatorTuning.frequency(v.note));
        });
}

void JPSynth::setDetune(host_float detune)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setDetune(detune);
        });
}

void JPSynth::setCarrierOscillatorType(CarrierOscillatiorType carrierType)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setCarrierOscillatorType(carrierType);
        });
}

void JPSynth::setModulatorOscillatorType(ModulatorOscillatorType modulatorType)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setModulatorOscillatorType(modulatorType);
        });
}

void JPSynth::setNoiseType(NoiseType noiseType)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setNoiseType(noiseType);
        });
}

void JPSynth::setModulation(host_float idx)
{
    host_float index = midi.normalizeModulation(idx) * 20;

    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setModIndex(index);
        });
}

void JPSynth::setPitchBend(host_float bend)
{
    dsp_float b = midi.normalizePitchBend(bend) * 1200.0;

    carrierTuning.setFinetune(b);
    modulatorTuning.setFinetune(b);

    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setCarrierFrequency(carrierTuning.frequency(v.note));
            v.jpvoice.setModulatorFrequency(modulatorTuning.frequency(v.note));
        });
}

void JPSynth::setNumVoices(int numVoices)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setNumVoices(numVoices);
        });
}

void JPSynth::setSyncEnabled(bool enable)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setSyncEnabled(enable);
        });
}

void JPSynth::setFilterMode(FilterMode mode)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFilterMode(mode);
        });
}

void JPSynth::setFeedbackCarrier(host_float fb)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFeedbackCarrier(fb);
        });
}

void JPSynth::setFeedbackModulator(host_float fb)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFeedbackModulator(fb);
        });
}

void JPSynth::setFilterCutoff(host_float f)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFilterCutoff(f);
        });
}

void JPSynth::setFilterResonance(host_float r)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFilterResonance(r);
        });
}

void JPSynth::setFilterDrive(host_float d)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFilterDrive(d);
        });
}

void JPSynth::setFilterADSR(ADSRParams adsr)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFilterADSR(adsr);
        });
}

// Sets the amplification envelope parameters
void JPSynth::setAmpADSR(ADSRParams adsr)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setAmpADSR(adsr);
        });
}

void JPSynth::linkADSR(bool enable)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.linkADSR(enable);
        });
}

void JPSynth::setADSROneshot(bool enable)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setADSROneshot(enable);
        });
}

void JPSynth::processBlock()
{
    // Signal next block processing for DSP (optional, statistic, debug)
    DSP::nextBlock();

    for (auto *voice : allocator.getVoices())
    {
        voice->jpvoice.computeSamples();
    }

    mixerL.mix();
    mixerR.mix();
}

void JPSynth::createVoices()
{
    allocator.clear();

    for (size_t i = 0; i < voiceCount; ++i)
    {
        std::unique_ptr<SynthVoice> voice = std::make_unique<SynthVoice>();

        voice->jpvoice.initialize();

        // Transfer ownership to allocator
        allocator.add(std::move(voice));
    }
}