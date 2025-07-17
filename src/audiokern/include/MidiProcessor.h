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
    void setLoudnessScale(host_float scale);

    // Normalizes velocity to -1/1
    host_float normalizeVelocity(host_float velocity);

    // Normalizes velocity to -1/1 (RMS scaled)
    host_float normalizeVelocityRMS(host_float velocity);

    // Normalizes pitch bend to -1/1
    host_float normalizePitchBend(host_float bend);

    // Normalizes pitch bend to -1/1 (RMS scaled)
    host_float normalizePitchBendRMS(host_float bend);

    // Normalizes modulation value to -1/1
    host_float normalizeModulation(host_float mod);

    // Normalizes modulation value to -1/1 (RMS scaled)
    host_float normalizeModulationRMS(host_float mod);

private:
    host_float loudnessScale;
};
