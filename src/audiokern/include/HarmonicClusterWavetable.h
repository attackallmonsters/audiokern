#pragma once

#include "WavetableOscillator.h"
#include "WaveformGenerator.h"

// Harmonic cluster oscillator using wavetable lookup
class HarmonicClusterWavetable : public WavetableOscillator
{
public:
    HarmonicClusterWavetable() : WavetableOscillator("cluster") {};

protected:
    // Fills the given buffer with one cycle of a harmonic cluster waveform
    void createWavetable(DSPBuffer &buffer, dsp_float frequency) override;
};
