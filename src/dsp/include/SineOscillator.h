// SineOscillator.h

#pragma once

#include "Oscillator.h"
#include <vector>
#include <cmath>
#include "dsp_types.h"
#include "dsp_math.h"

// Sine oscillator generates sine waveform
class SineOscillator : public Oscillator
{
public:
    // Default constructor
    SineOscillator();

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