#include "Mixer.h"

void Mixer::create(size_t count, host_float *bufL, host_float *bufR)
{
    bufferCount = count;

    outputBufferL = bufL;
    outputBufferR = bufR;

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

DSPSampleBuffer &Mixer::getBufferL(size_t index)
{
    return buffersL[index];
}

DSPSampleBuffer &Mixer::getBufferR(size_t index)
{
    return buffersR[index];
}

void Mixer::clear()
{
    buffersL.clear();
    buffersR.clear();

    outputBufferL = nullptr;
    outputBufferR = nullptr;
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
