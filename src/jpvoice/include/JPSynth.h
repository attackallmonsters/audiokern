#pragma once

#include "VoiceManager.h"
#include "JPVoice.h"
#include "DSPSampleBuffer.h"
#include "Mixer.h"

class JPSynth : public VoiceManager<JPVoice, JPSynth>
{
public:
    // Static instance
    static JPSynth &instance();

    // Initializes the instance
    void initialize(host_float *bufL, host_float *bufR);

    // Handles a note in event
    void noteIn(int note, host_float velocity);

    // Creates a new JPVoice
    std::unique_ptr<JPVoice> createVoice() override;

    // Note on for JPVoice
    void noteOn(JPVoice *voice, int note) override;

    // Note off event for JPVoice
    void noteOff(JPVoice *voice) override;

    // Computes the samples of all voices
    void computeSamples();

    // The number of voices
    const int voiceCount = 6;

private:
    Mixer mixerL;
    Mixer mixerR;
};