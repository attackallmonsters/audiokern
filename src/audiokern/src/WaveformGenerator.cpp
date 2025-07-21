#include "WaveformGenerator.h"

void WaveformGenerator::generateWavetable(DSPBuffer &buffer,
                                          dsp_float baseFrequency,
                                          AmplitudeFunction amplitudeFunc,
                                          dsp_float harmonicBoost)
{
    size_t size = buffer.size();

    // Check for invalid input (no size, zero freq/sampleRate)
    if (size == 0 || baseFrequency <= 0.0)
    {
        DSP::log("WaveformGenerator::generateWavetable failed: invalid buffer size or invalid frequency");
        return;
    }

    // Nyquist frequency: we only include harmonics below this threshold
    const dsp_float nyquist = 0.5 * DSP::sampleRate;

    // Maximum number of harmonics allowed without aliasing
    int harmonics = static_cast<int>(nyquist / baseFrequency * (1 + clamp(harmonicBoost, 0, 1) * 9));

    // Loop over each sample index in the buffer
    for (size_t i = 0; i < size; ++i)
    {
        // Convert index to phase in range [0.0, 1.0)
        dsp_float phase = static_cast<dsp_float>(i) / static_cast<dsp_float>(size - 1);

        dsp_float sample = 0.0;

        // Sum up all harmonics (from 1st to harmonics)
        for (int n = 1; n <= harmonics; ++n)
        {
            // Get amplitude for harmonic n
            dsp_float amp = amplitudeFunc(n);

            // Add sine component for harmonic n at current phase
            sample += amp * std::sin(dsp_math::DSP_2PI * n * phase);
        }

        // Store computed sample in buffer (converted to DSP format)
        buffer[i] = DSP::zeroSubnormals(sample);
    }

    // Nomralisation
    // Find peak
    dsp_float peak = 0.0;
    for (size_t i = 0; i < size; ++i)
        peak = std::max(peak, std::abs(buffer[i]));

    // Normalize to ±1.0
    if (peak > 0.0)
    {
        for (size_t i = 0; i < size; ++i)
            buffer[i] /= peak;
    }
}
