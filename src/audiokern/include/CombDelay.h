#pragma once

#include "DSPObject.h"
#include "RingBlockBuffer.h"
#include "DSPSampleBuffer.h"
#include "ParamFader.h"
#include <vector>

class CombDelay : public DSPObject
{
public:
    // Constructor
    explicit CombDelay();

    // Prepare buffers and internal state
    void initialize() override;

    // Set delay time in milliseconds
    void setTime(dsp_float timeMS);

    // Set feedback amount (0.0 – 0.99 typical)
    void setFeedback(dsp_float fb);

    // Set damping lowpass filter frequency (Hz)
    void setDamping(dsp_float freqHz);

    // Processes the next block
    void processBlock();

    // Internal delay buffers
    RingBlockBuffer delayBuffer;

    // Output buffers for left and right channel
    DSPSampleBuffer outputBufferL;
    DSPSampleBuffer outputBufferR;

private:
    // DSP callback
    static void processBlock(DSPObject* dsp);

    size_t writeIndex = 0;
    size_t delaySamples = 0;

    dsp_float feedback = 0.7;
    dsp_float dampingCoeff = 0.5;
    dsp_float dampingStateL = 0.0;
    dsp_float dampingStateR = 0.0;

    ParamFader paramFader;
};
