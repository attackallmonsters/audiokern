#include "MirrorWavetable.h"
#include <cmath>

// Amplitude function for WaveGenerator
static dsp_float mirrorAmplitude(int harmonic)
{
    return 1.0 / (std::abs(harmonic - 10) + 1);
}

void MirrorWavetable::createWavetable(DSPBuffer &buffer, dsp_float frequency)
{
    // Fill one full waveform cycle (0 to 2π) across the buffer
    WaveformGenerator::generateWavetable(buffer, frequency, mirrorAmplitude, 0.5);
}
