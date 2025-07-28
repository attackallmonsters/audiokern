// TriangleOscillator.h

#pragma once

#include "Oscillator.h"
#include "dsp_types.h"
#include <vector>
#include <cmath>

// Triangle oscillator generates triangle waveform
class TriangleOscillator : public Oscillator
{
public:
    // Default constructor
    TriangleOscillator();

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