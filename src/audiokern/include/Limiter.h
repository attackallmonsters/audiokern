#pragma once

#include "SoundProcessor.h"
#include "DSPSampleBuffer.h"
#include "dsp_types.h"
#include "clamp.h"
#include <vector>
#include <cmath>
#include <algorithm>

class Limiter : public SoundProcessor
{
public:
    // Ctor
    Limiter();

    // Sets the amplitude threshold in dBFS (-100 - 0)
    void setThreshold(dsp_float thres);

    // Sets the release time in milliseconds
    void setReleaseTime(dsp_float releaseMs);

    // Sets lookahead in milliseconds
    void setLookaheadTime(dsp_float ms);

    // Resets the limiter
    void reset();

protected:
    // Initializes the instance
    void initializeProcessor() override;

private:
    // Processes data in bufferL, buffer R
    static void processBlock(DSPObject *dsp);

    // Same same
    void processBlock();

    // Circular buffer for lookahead
    std::vector<std::pair<host_float, host_float>> lookaheadBuffer;

    dsp_float threshold;    // Amplitude threshold
    dsp_float releaseCoeff; // Exponential smoothing coefficient for release
    int lookaheadSamples;   // Number of samples for lookahead
    int bufferIndex;        // Write index into circular buffer
    dsp_float gain;         // Current gain applied to signal
};
