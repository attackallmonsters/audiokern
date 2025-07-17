#include "ModuloWavetable.h"
#include <cmath>

// Amplitude function for WaveGenerator
static dsp_float moduloAmplitude(int harmonic)
{
    int mod = 4 + static_cast<int>(std::floor(3 * std::sin(harmonic * 0.2)));

    return (harmonic % mod == 1) ? 0.7 / harmonic : 0.0;
}

void ModuloWavetable::createWavetable(DSPBuffer &buffer, dsp_float frequency)
{
    // Fill one full waveform cycle (0 to 2π) across the buffer
    WaveformGenerator::generateWavetable(buffer, frequency, moduloAmplitude, 0.5);
}
