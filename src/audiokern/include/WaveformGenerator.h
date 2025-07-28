#pragma once

#include "DSPBuffer.h"
#include "DSP.h"
#include "dsp_types.h"
#include "dsp_math.h"
#include "clamp.h"
#include <cmath>
#include <functional>

// The WaveformGenerator class creates a band-limited wavetable
// using additive synthesis. It does NOT store any waveform types internally.
// A user-defined amplitude function determines the harmonic structure.
class WaveformGenerator
{
public:
    // The amplitude function defines the amplitude of each harmonic (n)
    // up to maxHarmonics. It is user-supplied and determines the waveform shape.
    using AmplitudeFunction = std::function<dsp_float(int harmonic)>;

    // Fills the given buffer with one full waveform cycle (0–1 phase range)
    // using additive synthesis. Only harmonics below Nyquist are included.
    // Parameters:
    // - buffer: the target wavetable buffer (will be resized if needed)
    // - baseFrequency: the fundamental frequency (used to limit harmonics)
    // - sampleRate: the system's sample rate
    // - amplitudeFunc: user-supplied function that returns harmonic amplitudes
    // - harmonicBoost: 0 - 1 (optional aliasing)
    static void generateWavetable(DSPBuffer &buffer,
                                  dsp_float baseFrequency,
                                  AmplitudeFunction amplitudeFunc,
                                  dsp_float harmonicBoost = 0);
};
