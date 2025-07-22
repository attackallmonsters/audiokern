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

    // Internal delay buffers
    RingBlockBuffer delayBuffer;

protected:
    // Prepare buffers and internal state
    DSPUsage initializeObject() override;
    void onBuffersCreated() override;

private:
    // DSP callback
    static void processBlock(DSPObject *dsp);

    dsp_float feedback = 0.7;
    dsp_float dampingCoeff = 0.5;
    dsp_float dampingStateL = 0.0;
    dsp_float dampingStateR = 0.0;

    ParamFader paramFader;
};
