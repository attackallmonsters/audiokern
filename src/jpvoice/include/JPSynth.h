#pragma once

#include "JPVoice.h"
#include "DSPSampleBuffer.h"
#include "Mixer.h"
#include "TuningSystem.h"
#include "VoiceAllocator.h"
#include "MidiProcessor.h"

struct SynthVoice
{
    JPVoice jpvoice;
};

class JPSynth
{
public:
    // Static instance
    static JPSynth &instance();

    // Initializes the instance
    void initialize(host_float *bufL, host_float *bufR);

    // Handles a note in event
    void noteIn(int note, host_float velocity);

    // Computes the samples of all voices
    void processBlock();

private:
    // Creates the voices
    void createVoices();

    // The currenttly active voice
    SynthVoice *currentVoice;

    // Poly voice allocation management
    VoiceAllocator<SynthVoice> allocator;

    // Mixer left channel
    Mixer mixerL;

    // Mixer right channel
    Mixer mixerR;

    // Tuning system carrier
    TuningSystem carrierTuning;

    // Tuning system modulator
    TuningSystem modulatorTuning;

    // MIDI support
    MidiProcessor midi;

    // The number of voices
    const size_t voiceCount = 6;
};