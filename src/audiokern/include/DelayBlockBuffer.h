#pragma once

#include "DSP.h"
#include "dsp_types.h"
#include <vector>

// Simple and efficient ring buffer based on DSP blocks 
// with size defined via time in ms (converted into whole blocks)
class DelayBlockBuffer
{
public:
    DelayBlockBuffer();

    // Set desired buffer duration in milliseconds 
    void setTime(dsp_float timeMS);

    // Allocate and clear the buffer according to time and DSP::sampleRate
    void initialize();

    // Pushes a sample buffer into the buffer and the delayed buffer to the outbut buffer
    void push(const DSPSampleBuffer &blockL, const DSPSampleBuffer &blockR);

    // Return the current buffer size in blocks
    size_t size() const;

    // Output buffers 
    DSPSampleBuffer outputBufferL;
    DSPSampleBuffer outputBufferR;

    // Feedback buffers
    DSPSampleBuffer feedbackBufferL;
    DSPSampleBuffer feedbackBufferR;

private:
    std::vector<host_float> bufferL;
    std::vector<host_float> bufferR;

    size_t writeIndex;
    size_t bufferSize;
    size_t blockCount;
    dsp_float delayTimeMs;
};
