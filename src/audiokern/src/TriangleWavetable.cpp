#include "TriangleWavetable.h"
#include <cmath>

// Amplitude function for WaveGenerator
static dsp_float trianlgeAmplitude(int harmonic)
{
    if (harmonic % 2 == 1)
    {
        double sign = ((harmonic - 1) / 2) % 2 == 0 ? 1.0 : -1.0;
        return sign * (1.0 / (harmonic * harmonic));
    }
    else
        return 0.0;
}

void TriangleWavetable::createWavetable(DSPBuffer &buffer, dsp_float frequency)
{
    // Fill one full waveform cycle (0 to 2π) across the buffer
    WaveformGenerator::generateWavetable(buffer, frequency, trianlgeAmplitude, 0.5);
}
