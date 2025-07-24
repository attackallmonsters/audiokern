#include "Mixer.h"

Mixer::Mixer()
{
    registerBlockProcessor(&Mixer::processBlock);
}

DSPUsage Mixer::initializeObject(size_t count)
{
    bufferCount = count;

    buffersL.clear();
    buffersL.resize(bufferCount);

    buffersR.clear();
    buffersR.resize(bufferCount);

    for (size_t i = 0; i < bufferCount; ++i)
    {
        buffersL[i].initialize("buffersL_" + std::to_string(i) + getName(), DSP::blockSize);
        buffersR[i].initialize("buffersR_" + std::to_string(i) + getName(), DSP::blockSize);
    }

    return DSPUsage::OutputOnly;
}

DSPSampleBuffer &Mixer::getInputBufferL(size_t index)
{
    return buffersL[index];
}

DSPSampleBuffer &Mixer::getInputBufferR(size_t index)
{
    return buffersR[index];
}

void Mixer::processBlock()
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

void Mixer::processBlock(DSPObject *dsp)
{
    Mixer *self = static_cast<Mixer *>(dsp);
    self->processBlock();
}