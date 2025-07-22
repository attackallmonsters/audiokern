#pragma once

#include "DSP.h"
#include "dsp_types.h"
#include <vector>

// Simple and efficient ring buffer based on DSP blocks 
// with size defined via time in ms (converted into whole blocks)
class RingBlockBuffer
{
public:
    RingBlockBuffer();

    // Set desired buffer duration in milliseconds 
    void setTime(dsp_float timeMSL, dsp_float timeMSR);

    // Allocate and clear the buffer according to time and DSP::sampleRate
    void initialize();

    // Pushes a sample buffer into the buffer and the delayed buffer to the outbut buffer
    void push(const DSPSampleBuffer &blockL, const DSPSampleBuffer &blockR);

    // Output buffers 
    DSPSampleBuffer outputBufferL;
    DSPSampleBuffer outputBufferR;

    // Feedback buffers
    DSPSampleBuffer feedbackBufferL;
    DSPSampleBuffer feedbackBufferR;

private:
    std::vector<host_float> bufferL;
    std::vector<host_float> bufferR;

    size_t writeIndexL, writeIndexR;
    size_t bufferSizeL, bufferSizeR;
    size_t blockCountL, blockCountR;
    dsp_float delayTimeMsL, delayTimeMsR;
};
