#pragma once

#include "DSPSampleBuffer.h"
#include "DSP.h"
#include <vector>
#include <cstddef>

class Mixer
{
public:
    Mixer() = default;

    // Initializes the mixer with a number of buffers
    void create(size_t count, host_float *bufL, host_float *bufR);

    // Returns a reference to a buffer for direct usage in sound generators
    DSPSampleBuffer& getBufferL(size_t index);
    DSPSampleBuffer& getBufferR(size_t index);

    // Removes the buffers
    void clear();

    // Mixes all buffers to the output buffer
    void mix();

private:
    size_t bufferCount;

    std::vector<DSPSampleBuffer> buffersL;
    std::vector<DSPSampleBuffer> buffersR;

    host_float* outputBufferL = nullptr;
    host_float* outputBufferR = nullptr;
};
