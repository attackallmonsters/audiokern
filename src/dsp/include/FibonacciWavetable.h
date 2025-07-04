#pragma once

#include "WavetableOscillator.h"
#include "WaveformGenerator.h"

// Fibonacci oscillator using wavetable lookup
class FibonacciWavetable : public WavetableOscillator
{
public:
    FibonacciWavetable() : WavetableOscillator("fibonacci") {};

protected:
    // Fills the given buffer with one cycle of a fibonacci waveform
    void createWavetable(XDSPBuffer &buffer, dsp_float frequency) override;
};
