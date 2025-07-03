#pragma once

#include "WavetableOscillator.h"
#include "WaveformGenerator.h"

// Square oscillator using wavetable lookup
class SquareWavetable : public WavetableOscillator
{
public:
    SquareWavetable() : WavetableOscillator("square") {};

protected:
    // Fills the given buffer with one cycle of a square waveform
    void createWavetable(DSPBuffer &buffer, dsp_float frequency) override;
};
