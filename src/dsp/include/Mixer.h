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
    void create(size_t count, host_float *buf);

    // Returns a reference to a buffer for direkt usage in sound generators
    DSPSampleBuffer& getBuffer(size_t index);

    // Removes the buffers
    void clear();

    // Mixes all buffers to the output buffer
    void mix();

private:
    std::vector<DSPSampleBuffer> buffers;
    host_float* outputBuffer = nullptr;
};
