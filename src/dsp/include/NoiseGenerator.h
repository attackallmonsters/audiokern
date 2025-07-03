#pragma once

#include "Oscillator.h"
#include "VoiceOptions.h"
#include "DSP.h"
#include "DSPObject.h"
#include <random>
#include "dsp_types.h"

// A subclass of Oscillator that generates noise instead of periodic waveforms.
// Supports white and pink noise generation.
class NoiseGenerator : public Oscillator
{
public:
    // Constructor: always initializes with white noise and distribution [-1, 1]
    NoiseGenerator();

    // Changes the current noise type (white or pink)
    void setType(NoiseType type);

private:
    // Sample calculation without looking up vtable
    static void generateSample(
        Oscillator *,
        const dsp_float &,
        const dsp_float &,
        dsp_float &,
        dsp_float &,
        const dsp_float &,
        const dsp_float &);

    // Random number generator for white noise source
    std::mt19937 rng;

    // Uniform distribution for generating white noise values in range [-1.0, 1.0]
    std::uniform_real_distribution<dsp_float> dist{-1.0, 1.0};

    // Current type of noise being generated
    NoiseType noiseType;

    // State variables for pink noise filter (Paul Kellet method)
    // These variables store previous filter states to create 1/f pink noise characteristics
    dsp_float b0 = 0.0, b1 = 0.0, b2 = 0.0;
    dsp_float b3 = 0.0, b4 = 0.0, b5 = 0.0, b6 = 0.0;
};
