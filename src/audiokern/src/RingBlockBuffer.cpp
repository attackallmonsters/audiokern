#include "RingBlockBuffer.h"

RingBlockBuffer::RingBlockBuffer()
{
}

void RingBlockBuffer::initialize(std::string name)
{
    bufferName = name;

    outputBufferL.initialize("outputBufferL" + bufferName, DSP::blockSize);
    outputBufferR.initialize("outputBufferR" + bufferName, DSP::blockSize);

    feedbackBufferL.initialize("feedbackBufferL" + bufferName, DSP::blockSize);
    feedbackBufferR.initialize("feedbackBufferR" + bufferName, DSP::blockSize);

    std::fill(bufferL.begin(), bufferL.end(), 0.0);
    std::fill(bufferR.begin(), bufferR.end(), 0.0);

    writeIndexL = writeIndexR = 0;

    setMaxTime(5000.0);
    setTime(1.0, 1.0);
}

void RingBlockBuffer::setMaxTime(host_float timeMS)
{
    maxTime = clampmin(timeMS, 0.0);

    size_t rawSize = static_cast<size_t>((maxTime / 1000.0) * DSP::sampleRate);

    if (rawSize < DSP::blockSize)
        rawSize = DSP::blockSize;

    bufferSize = ((rawSize + DSP::blockSize - 1) / DSP::blockSize) * DSP::blockSize;

    bufferL.resize(bufferSize);
    bufferR.resize(bufferSize);
}

void RingBlockBuffer::setTime(host_float timeMSL, host_float timeMSR)
{
    delayTimeMsL = clamp(timeMSL, 0.0, maxTime);
    delayTimeMsR = clamp(timeMSR, 0.0, maxTime);

    size_t rawSizeL = static_cast<size_t>((delayTimeMsL / 1000.0) * DSP::sampleRate);
    size_t rawSizeR = static_cast<size_t>((delayTimeMsR / 1000.0) * DSP::sampleRate);

    bufferSizeL = ((rawSizeL + DSP::blockSize - 1) / DSP::blockSize) * DSP::blockSize;
    bufferSizeR = ((rawSizeR + DSP::blockSize - 1) / DSP::blockSize) * DSP::blockSize;

    blockCountL = bufferSizeL < DSP::blockSize ? 1 : bufferSizeL / DSP::blockSize;
    blockCountR = bufferSizeR < DSP::blockSize ? 1 : bufferSizeR / DSP::blockSize;

    writeIndexL = 0;
}

void RingBlockBuffer::push(const DSPSampleBuffer &blockL, const DSPSampleBuffer &blockR)
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        bufferL[writeIndexL + i] = blockL[i] + feedbackBufferL[i];
        bufferR[writeIndexR + i] = blockR[i] + feedbackBufferR[i];
    }

    // Next write index is the oldest block in buffer
    writeIndexL += DSP::blockSize;
    writeIndexR += DSP::blockSize;

    if (writeIndexL >= bufferSizeL)
    {
        writeIndexL = 0;
    }

    if (writeIndexR >= bufferSizeR)
    {
        writeIndexR = 0;
    }

    outputBufferL.copy(&bufferL[writeIndexL]);
    outputBufferR.copy(&bufferR[writeIndexR]);
}

void RingBlockBuffer::clear()
{
    std::fill(bufferL.begin(), bufferL.end(), 0.0);
    std::fill(bufferR.begin(), bufferR.end(), 0.0);
    
    feedbackBufferL.fill(0.0);
    feedbackBufferR.fill(0.0);
}