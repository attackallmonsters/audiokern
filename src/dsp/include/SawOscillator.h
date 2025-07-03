// sawOscillator.h

#pragma once

#include "Oscillator.h"
#include <vector>
#include <cmath>
#include "dsp_types.h"

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
        const dsp_float &,
        const dsp_float &,
        dsp_float &,
        dsp_float &,
        const dsp_float &,
        const dsp_float &);
};