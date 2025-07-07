#include "BitWavetable.h"
#include <cmath>

// Amplitude function for WaveGenerator
static dsp_float bitAmplitude(int harmonic)
{
    return ((harmonic & 5) == 5) ? 1.0 / harmonic : 0.0;
}

void BitWavetable::createWavetable(DSPBuffer &buffer, dsp_float frequency)
{
    // Fill one full waveform cycle (0 to 2π) across the buffer
    WaveformGenerator::generateWavetable(buffer, frequency, bitAmplitude, 0.5);
}
