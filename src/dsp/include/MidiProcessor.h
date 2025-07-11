#pragma once

#include "dsp_types.h"
#include "clamp.h"
#include <cmath>

class MidiProcessor
{
public:
    // MIDI initialization
    void initialize();

    // Sets the global loudness scale
    void setLoudnessScale(host_float s);

    // Converts MIDI velocity to DSP gain
    host_float rmsVelocity(int velocity);

private:
    host_float loudnessScale;
};
