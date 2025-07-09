#include "Mixer.h"

void Mixer::create(size_t count, host_float *buf)
{
    outputBuffer = buf;
    buffers.clear();
    buffers.resize(count);

    for (auto& buf : buffers)
    {
        buf.create(DSP::blockSize);
    }
}

DSPSampleBuffer& Mixer::getBuffer(size_t index)
{
    return buffers[index];
}

void Mixer::clear()
{
    buffers.clear();
    outputBuffer = nullptr;
}

void Mixer::mix()
{
    for (const auto& buf : buffers)
    {
        for (size_t i = 0; i < DSP::blockSize; ++i)
        {
            outputBuffer[i] += buf[i];
        }
    }
}
