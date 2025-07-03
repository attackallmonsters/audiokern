#pragma once

#include "WavetableOscillator.h"
#include "WaveformGenerator.h"

// Triangle oscillator using wavetable lookup
class TriangleWavetable : public WavetableOscillator
{
public:
    TriangleWavetable() : WavetableOscillator("triangle") {};

protected:
    // Fills the given buffer with one cycle of a trianlge waveform
    void createWavetable(DSPBuffer &buffer, dsp_float frequency) override;
};
