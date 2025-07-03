#include "HarmonicClusterWavetable.h"
#include <cmath>

// Amplitude function for WaveGenerator
static dsp_float clusterAmplitude(int harmonic)
{
    double mask = 0.5 * (std::sin(harmonic * 0.4) + 1.0); // in [0,1]
    return (mask > 0.8) ? 0.6 / harmonic : 0.1 / harmonic;
}

void HarmonicClusterWavetable::createWavetable(DSPBuffer &buffer, dsp_float frequency)
{
    // Fill one full waveform cycle (0 to 2π) across the buffer
    WaveformGenerator::generateWavetable(buffer, frequency, clusterAmplitude, 0.5);
}
