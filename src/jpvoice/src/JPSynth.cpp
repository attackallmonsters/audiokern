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
        // allocator.forEachVoice(
        //     [&](auto &v)
        //     {
        //         v.jpvoice.setCarrierFrequency(carrierTuning.frequency(note));
        //         v.jpvoice.setModulatorFrequency(modulatorTuning.frequency(note));
        //         v.jpvoice.setAmpGain(midi.rmsVelocity(velocity));
        //     });

        DSP::log("F: %fHz; N: %d", carrierTuning.frequency(note), note);
        currentVoice = allocator.allocate(note);
        currentVoice->jpvoice.setCarrierFrequency(carrierTuning.frequency(note));
        currentVoice->jpvoice.setModulatorFrequency(modulatorTuning.frequency(note));
        currentVoice->jpvoice.setAmpGain(midi.rmsVelocity(velocity));
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

void JPSynth::processBlock()
{
    // Signal next block processing for DSP
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