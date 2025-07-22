#pragma once

#include "DSP.h"
#include "dsp_types.h"
#include "clamp.h"
#include <vector>

// Simple and efficient ring buffer based on DSP blocks 
// with size defined via time in ms (converted into whole blocks)
class RingBlockBuffer
{
public:
    RingBlockBuffer();

    // Sets the maximum buffer time
    void setMaxTime(host_float timeMS);

    // Set desired buffer duration in milliseconds 
    void setTime(host_float timeMSL, host_float timeMSR);

    // Allocate and clear the buffer according to time and DSP::sampleRate
    void initialize();

    // Pushes a sample buffer into the buffer and the delayed buffer to the outbut buffer
    void push(const DSPSampleBuffer &blockL, const DSPSampleBuffer &blockR);

    // Clears all buffers
    void clear();

    // Output buffers 
    DSPSampleBuffer outputBufferL;
    DSPSampleBuffer outputBufferR;

    // Feedback buffers
    DSPSampleBuffer feedbackBufferL;
    DSPSampleBuffer feedbackBufferR;

private:
    std::vector<host_float> bufferL;
    std::vector<host_float> bufferR;

    size_t bufferSize, bufferSizeL, bufferSizeR;
    size_t writeIndexL, writeIndexR;
    size_t blockCountL, blockCountR;
    host_float maxTime, delayTimeMsL, delayTimeMsR;
};
