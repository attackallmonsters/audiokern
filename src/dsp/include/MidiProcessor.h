#pragma once

#include "dsp_types.h"
#include "TunigSystem.h"
#include <cmath>

class MidiProcessor
{
public:
    // MIDI initialization
    void initialize();

    // Converts a MIDI note number (0–127) to frequency in Hz
    dsp_float noteToFrequency(int note);

    // The tunig system to use (default 440Hz base, 12 half tones)
    TunigSystem tuningSystem;

private:
};
