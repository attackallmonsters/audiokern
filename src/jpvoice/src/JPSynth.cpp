#include "JPSynth.h"

JPSynth& JPSynth::instance()
{
    static JPSynth instance; // Wird beim ersten Aufruf erstellt
    return instance;
}

void JPSynth::initialize()
{
    voiceManager.initialize();
    
    voiceManager.setNumVoices(6);
    voiceManager.toAll(&JPVoice::initialize);
}

void JPSynth::noteIn(int note, host_float velocity)
{
    voiceManager.handleNote(note, velocity);
}