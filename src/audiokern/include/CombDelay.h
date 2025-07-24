#pragma once

#include "DSPObject.h"
#include "RingBlockBuffer.h"
#include "DSPSampleBuffer.h"
#include "ParamFader.h"
#include "dsp_math.h"
#include "clamp.h"
#include <vector>

class CombDelay : public DSPObject
{
public:
    // Constructor
    explicit CombDelay();

    // Set delay maximum time in milliseconds
    void setMaxTime(dsp_float timeMS);

    // Set delay time in milliseconds
    void setTime(dsp_float timeMS);

    // Set feedback amount (0.0 – 0.99 typical)
    void setFeedback(dsp_float fb);

    // Set damping lowpass filter frequency (Hz)
    void setDamping(dsp_float freqHz);

    // Processes the next block
    void processBlock();

    // Pushes a sample buffer into the buffer and the delayed buffer to the outbut buffer
    void push(const DSPSampleBuffer &blockL, const DSPSampleBuffer &blockR);

protected:
    // Prepare buffers and internal state
    DSPUsage initializeObject() override;
    void onOutputBuffersAssigned() override;

private:
    // DSP callback
    static void processBlock(DSPObject *dsp);

    // Filter settings
    dsp_float feedback;
    dsp_float dampingCoeff;
    dsp_float dampingStateL;
    dsp_float dampingStateR;

    // Internal delay buffers
    RingBlockBuffer delayBuffer;

    // Parameter volume fader
    ParamFader paramFader;
};
