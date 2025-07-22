#pragma once

#include "DSPObject.h"
#include "RingBlockBuffer.h"
#include "DSPSampleBuffer.h"
#include "ParamFader.h"
#include "dsp_math.h"
#include "clamp.h"
#include <vector>

class Delay : public DSPObject
{
public:
    // Constructor
    explicit Delay();

    // Set delay time in milliseconds
    void setTime(dsp_float timeMSL, dsp_float timeMSR);

    // Set feedback amount (0.0 – 0.99 typical)
    void setFeedback(dsp_float fbL, host_float fbR);

    // Processes the next block
    void processBlock();

    // Internal delay buffers
    RingBlockBuffer delayBuffer;

protected:
    // Prepare buffers and internal state
    DSPObjectUsage initializeComponent() override;

    // Called when buffers have been created
    void onBuffersCreated() override;

private:
    // DSP callback
    static void processBlock(DSPObject *dsp);

    host_float feedbackL, feedbackR;

    ParamFader paramFader;
};
