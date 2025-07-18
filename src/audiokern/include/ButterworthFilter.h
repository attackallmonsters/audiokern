#pragma once

#include "DSPObject.h"
#include "DSPSampleBuffer.h"
#include "VoiceOptions.h"
#include "dsp_types.h"
#include "dsp_math.h"
#include "clamp.h"
#include <cmath>


class ButterworthFilter : public DSPObject
{
public:
    // Constructor
    explicit ButterworthFilter();

    // Initialize filter
    void initialize() override;

    // Sets the cutoff frequency
    void setCutoffFrequency(dsp_float freq);

    // Reset filter states
    void reset();

    // Set the filter mode (Lowpass or Highpass)
    void setFilterMode(FilterMode mode);

    // Output buffers for left and right channel
    DSPSampleBuffer bufferL;
    DSPSampleBuffer bufferR;

private:
    // Cutoff frequency
    dsp_float cutoffFrequency;

    // Internal filter state for left channel
    host_float z1L = 0.0, z2L = 0.0;

    // Internal filter state for right channel
    host_float z1R = 0.0, z2R = 0.0;

    // Current filter mode
    FilterMode filterMode = FilterMode::LP;

    // DSP processing function
    static void processBlock(DSPObject *dsp);
};
