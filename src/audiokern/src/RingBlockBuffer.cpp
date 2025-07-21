#include "RingBlockBuffer.h"

// Constructor: only initialize members
RingBlockBuffer::RingBlockBuffer()
{
}

// Allocate buffer and clear content
void RingBlockBuffer::initialize()
{
    outputBufferL.create(DSP::blockSize);
    outputBufferR.create(DSP::blockSize);

    outputBufferL.fill(0.0);
    outputBufferR.fill(0.0);

    feedbackBufferL.create(DSP::blockSize);
    feedbackBufferR.create(DSP::blockSize);

    feedbackBufferL.fill(0.0);
    feedbackBufferR.fill(0.0);

    std::fill(bufferL.begin(), bufferL.end(), 0.0);
    std::fill(bufferR.begin(), bufferR.end(), 0.0);

    writeIndex = 0;
    setTime(1);
}

// Set target buffer length in milliseconds (no allocation yet)
void RingBlockBuffer::setTime(dsp_float timeMS)
{
    delayTimeMs = timeMS;

    size_t rawSize = static_cast<size_t>((delayTimeMs / 1000.0) * DSP::sampleRate);

    if (rawSize < DSP::blockSize)
        rawSize = DSP::blockSize;

    bufferSize = ((rawSize + DSP::blockSize - 1) / DSP::blockSize) * DSP::blockSize;

    if (bufferL.size() == bufferSize)
        return;

    bufferL.resize(bufferSize);
    bufferR.resize(bufferSize);

    blockCount = bufferSize / DSP::blockSize;

    writeIndex = 0;
}

#include <stdexcept>
// Push a sample into the circular buffer
void RingBlockBuffer::push(const DSPSampleBuffer &blockL, const DSPSampleBuffer &blockR)
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        bufferL[writeIndex + i] = blockL[i] + feedbackBufferL[i];
        bufferR[writeIndex + i] = blockR[i] + feedbackBufferR[i];
    }

    // Next write index is the oldest block in buffer
    writeIndex += DSP::blockSize;

    if (writeIndex >= bufferSize)
    {
        writeIndex = 0;
    }

    outputBufferL.copy(&bufferL[writeIndex]);
    outputBufferR.copy(&bufferR[writeIndex]);
}

// Return current buffer blocks
size_t RingBlockBuffer::size() const
{
    return blockCount;
}
