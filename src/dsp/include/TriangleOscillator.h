// TriangleOscillator.h

#pragma once

#include "Oscillator.h"
#include "dsp_types.h"

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
        const dsp_float &,
        const dsp_float &,
        dsp_float &,
        dsp_float &,
        const dsp_float &,
        const dsp_float &);
};