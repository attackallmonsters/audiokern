#pragma once

#include "WavetableOscillator.h"
#include "WaveformGenerator.h"

// Bitcrush oscillator using wavetable lookup
class BitWavetable : public WavetableOscillator
{
public:
    BitWavetable() : WavetableOscillator("bit") {};

protected:
    // Fills the given buffer with one cycle of a bitchrush waveform
    void createWavetable(DSPBuffer &buffer, dsp_float frequency) override;
};
