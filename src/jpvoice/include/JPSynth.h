#pragma once

#include "VoiceManager.h"
#include "JPVoice.h"

class JPSynth : public VoiceManager<JPVoice, JPSynth>
{
public:
    // Static instance
    static JPSynth &instance();

    // Initializes the instance
    void initialize();

    // Handles a note in event
    void noteIn(int note, host_float velocity);

private:
    // Direct access to voice manager
    VoiceManager<JPVoice, JPSynth> voiceManager;
};