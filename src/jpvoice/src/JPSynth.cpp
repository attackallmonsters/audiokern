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

    VoiceManager::initialize();

    setNumVoices(voiceCount);

    mixerL.clear();
    mixerR.clear();

    mixerL.create(voiceCount, bufL);
    mixerR.create(voiceCount, bufR);

    for (int i = 0; i < voiceCount; ++i)
    {
        JPVoice *voice = allocator.getVoice(i);

        voice->initialize();

        voice->setOutputBuffer(mixerL.getBuffer(i), mixerR.getBuffer(i));
    }

    DSP::log("=====> jpvoice initialized");
}

void JPSynth::noteIn(int note, host_float velocity)
{
    handleNote(note, velocity);
}

std::unique_ptr<JPVoice> JPSynth::createVoice()
{
    return std::make_unique<JPVoice>();
}

void JPSynth::computeSamples()
{
    for (auto *voice : allocator.getVoices())
    {
        voice->computeSamples();
    }

    mixerL.mix();
    mixerR.mix();
}

void JPSynth::noteOn(JPVoice *voice, int note)
{
    voice->setFrequency(tuningSystem.calculateIntervallFrequency(note));
    voice->playNote();

    tuningSystem.setFinetune(10);
}

void JPSynth::noteOff(JPVoice *voice)
{
    voice->stopNote();
}