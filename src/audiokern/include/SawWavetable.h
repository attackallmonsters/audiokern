#pragma once

#include "WavetableOscillator.h"
#include "WaveformGenerator.h"

// Sine oscillator using wavetable lookup
class SawWavetable : public WavetableOscillator
{
public:
    SawWavetable() : WavetableOscillator("saw") {};

protected:
    // Fills the given buffer with one cycle of a sine waveform
    void createWavetable(DSPBuffer &buffer, dsp_float frequency) override;
};
