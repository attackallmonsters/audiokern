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
    void initialize(size_t count);

    // Returns a reference to a buffer for direct input from sound generators
    DSPSampleBuffer &getInputBufferL(size_t index);
    DSPSampleBuffer &getInputBufferR(size_t index);

    // Mixes all buffers to the output buffer
    void mix();

    DSPSampleBuffer outputBufferL;
    DSPSampleBuffer outputBufferR;

private:
    size_t bufferCount;

    std::vector<DSPSampleBuffer> buffersL;
    std::vector<DSPSampleBuffer> buffersR;
};
