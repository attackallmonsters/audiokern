#include "SquareWavetable.h"
#include <cmath>

// Amplitude function for WaveGenerator
static dsp_float squareAmplitude(int harmonic)
{
    if (harmonic % 2 == 1)
        return 1.0 / harmonic;
    else
        return 0.0;
}

void SquareWavetable::createWavetable(DSPBuffer &buffer, dsp_float frequency)
{
    // Fill one full waveform cycle (0 to 2π) across the buffer
    WaveformGenerator::generateWavetable(buffer, frequency, squareAmplitude, 0.5);
}
