#pragma once

#include "DSPObject.h"
#include "VoiceOptions.h"
#include "DSP.h"
#include "DSPObject.h"
#include <random>
#include "dsp_types.h"

// A subclass of Oscillator that generates noise instead of periodic waveforms.
// Supports white and pink noise generation.
class NoiseOscillator : public DSPObject
{
public:
    // Constructor: always initializes with white noise and distribution [-1, 1]
    NoiseOscillator();

    // Changes the current noise type (white or pink)
    void setType(NoiseType type);

protected:
    // Initializes the oscillator
    DSPUsage initializeObject() override;

private:
    // Next sample block generation one voice
    static void processBlockWhite(DSPObject *dsp);
    static void processBlockPink(DSPObject *dsp);

    // Random number generator for white noise source
    std::mt19937 rng;

    // Uniform distribution for generating white noise values in range [-1.0, 1.0]
    std::uniform_real_distribution<host_float> dist{-1.0, 1.0};

    // State variables for pink noise filter (Paul Kellet method)
    // These variables store previous filter states to create 1/f pink noise characteristics
    host_float b0 = 0.0, b1 = 0.0, b2 = 0.0;
    host_float b3 = 0.0, b4 = 0.0, b5 = 0.0, b6 = 0.0;
};
