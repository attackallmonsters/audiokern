#pragma once

#include "DSPSampleBuffer.h"
#include "DSPObject.h"
#include <vector>
#include <cstddef>

class Mixer : public DSPObject
{
public:
    Mixer();

    // Initializes the mixer with a number of buffers
    void initialize(size_t count);

    // Returns a reference to a buffer for direct input from sound generators
    DSPSampleBuffer &getInputBufferL(size_t index);
    DSPSampleBuffer &getInputBufferR(size_t index);

    // Processes the next block of samples
    void processBlock();

    DSPSampleBuffer outputBufferL;
    DSPSampleBuffer outputBufferR;

private:
    // Static DSP callback used for processing audio blocks
    static void processBlock(DSPObject *dsp);

    size_t bufferCount;

    std::vector<DSPSampleBuffer> buffersL;
    std::vector<DSPSampleBuffer> buffersR;
};
