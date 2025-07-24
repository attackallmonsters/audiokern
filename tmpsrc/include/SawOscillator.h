// sawOscillator.h

#pragma once

#include "Oscillator.h"
#include "dsp_types.h"
#include <vector>
#include <cmath>

// Sawtooth oscillator generates sawtooth waveform
class SawOscillator : public Oscillator
{
public:
    // Default constructor
    SawOscillator();

private:
    // Sample calculation without looking up vtable
    static void generateSample(
        Oscillator *,
        const host_float &,
        const host_float &,
        host_float &,
        host_float &,
        const host_float &,
        const host_float &);
};