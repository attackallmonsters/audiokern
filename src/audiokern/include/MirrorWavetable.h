#pragma once

#include "WavetableOscillator.h"
#include "WaveformGenerator.h"

// Mirror oscillator using wavetable lookup
class MirrorWavetable : public WavetableOscillator
{
public:
    MirrorWavetable() : WavetableOscillator("mirror") {};

protected:
    // Fills the given buffer with one cycle of a mirror waveform
    void createWavetable(DSPBuffer &buffer, dsp_float frequency) override;
};
