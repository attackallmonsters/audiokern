#include "SawWavetable.h"
#include <cmath>

// Amplitude function for WaveGenerator
static dsp_float sawAmplitude(int harmonic)
{
    return -1.0 / harmonic;
}

void SawWavetable::createWavetable(DSPBuffer &buffer, dsp_float frequency)
{
    // Fill one full waveform cycle (0 to 2π) across the buffer
    WaveformGenerator::generateWavetable(buffer, frequency, sawAmplitude, 0);
}
