#include "Mixer.h"

void Mixer::initialize(size_t count)
{
    bufferCount = count;

    outputBufferL.create(DSP::blockSize);
    outputBufferR.create(DSP::blockSize);

    buffersL.clear();
    buffersL.resize(bufferCount);

    buffersR.clear();
    buffersR.resize(bufferCount);

    for (size_t i = 0; i < bufferCount; ++i)
    {
        buffersL[i].create(DSP::blockSize);
        buffersR[i].create(DSP::blockSize);
    }
}

DSPSampleBuffer &Mixer::getInputBufferL(size_t index)
{
    return buffersL[index];
}

DSPSampleBuffer &Mixer::getInputBufferR(size_t index)
{
    return buffersR[index];
}

void Mixer::mix()
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        outputBufferL[i] = 0.0;
        outputBufferR[i] = 0.0;
    }

    for (size_t i = 0; i < bufferCount; ++i)
    {
        DSPSampleBuffer &bufL = buffersL[i];
        DSPSampleBuffer &bufR = buffersR[i];

        for (size_t l = 0; l < DSP::blockSize; ++l)
        {
            outputBufferL[l] += bufL[l];
            outputBufferR[l] += bufR[l];
        }
    }
}
