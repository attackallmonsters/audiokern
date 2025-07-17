#pragma once

#include "WavetableOscillator.h"
#include "WaveformGenerator.h"

// Modula 4 oscillator using wavetable lookup
class ModuloWavetable : public WavetableOscillator
{
public:
    ModuloWavetable() : WavetableOscillator("modulo") {};

protected:
    // Fills the given buffer with one cycle of a modula 4 waveform
    void createWavetable(DSPBuffer &buffer, dsp_float frequency) override;
};
