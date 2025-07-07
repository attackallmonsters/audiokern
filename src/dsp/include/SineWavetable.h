#pragma once

#include "WavetableOscillator.h"

// Sine oscillator using wavetable lookup
class SineWavetable : public WavetableOscillator
{
public:
    // Ctor
    SineWavetable() : WavetableOscillator("sine") {};

protected:
    // Fills the given buffer with one cycle of a sine waveform
    void createWavetable(DSPBuffer &buffer, dsp_float frequency) override;

private:
    // Indicated if the wavetable has been loaded.
    bool loaded = false;
};
