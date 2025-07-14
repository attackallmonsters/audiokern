#pragma once

#include "DSPObject.h"
#include "DSPSampleBuffer.h"
#include "dsp_types.h"
#include "clamp.h"
#include <vector>
#include <cmath>
#include <algorithm>

class Limiter : public DSPObject
{
public:
    // Ctor
    Limiter();

    // Initializes the instance
    void initialize();

    // Sets the amplitude threshold in dBFS (-100 - 0)
    void setThreshold(dsp_float thres);

    // Sets the release time in milliseconds
    void setReleaseTime(dsp_float releaseMs);

    // Sets lookahead in milliseconds
    void setLookaheadTime(dsp_float ms);

    // Resets the limiter
    void reset();

    // Output samples left
    DSPSampleBuffer outputBufferL;

    // Output samples right
    DSPSampleBuffer outputBufferR;

private:
    // Processes data in bufferL, buffer R
    static void processBlock(DSPObject *dsp);

    std::vector<std::pair<host_float, host_float>> lookaheadBuffer; // Circular buffer for lookahead

    dsp_float threshold;    // Amplitude threshold
    dsp_float releaseCoeff; // Exponential smoothing coefficient for release
    int lookaheadSamples;   // Number of samples for lookahead
    int bufferIndex;        // Write index into circular buffer
    dsp_float gain;         // Current gain applied to signal
};
