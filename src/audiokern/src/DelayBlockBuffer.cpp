#include "DelayBlockBuffer.h"

// Constructor: only initialize members
DelayBlockBuffer::DelayBlockBuffer()
{
    outputBufferL.create(DSP::blockSize);
    outputBufferR.create(DSP::blockSize);

    feedbackBufferL.create(DSP::blockSize);
    feedbackBufferR.create(DSP::blockSize);
}

// Set target buffer length in milliseconds (no allocation yet)
void DelayBlockBuffer::setTime(dsp_float timeMS)
{
    timeMs = timeMS;

    size_t rawSize = static_cast<size_t>((timeMs / 1000.0) * DSP::sampleRate);

    if (rawSize < DSP::blockSize)
        rawSize = DSP::blockSize;

    bufferSize = ((rawSize + DSP::blockSize - 1) / DSP::blockSize) * DSP::blockSize;

    if (bufferL.size() == bufferSize)
        return;

    bufferL.resize(bufferSize);
    bufferR.resize(bufferSize);

    blockCount = bufferSize / DSP::blockSize;
}

// Allocate buffer and clear content
void DelayBlockBuffer::initialize()
{
    writeIndex = readBlockIndex = 0;
    canRead = false;
    setTime(1);
}

// Push a sample into the circular buffer
void DelayBlockBuffer::push(const DSPSampleBuffer &blockL, const DSPSampleBuffer &blockR)
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        bufferL[writeIndex + i] = blockL[i] + feedbackBufferL[i];
        bufferR[writeIndex + i] = blockR[i] + feedbackBufferR[i];
    }

    writeIndex += DSP::blockSize;

    if (writeIndex >= bufferSize)
    {
        writeIndex = 0;
        canRead = true;
    }
}

// Writes the next blocks to output buffers
void DelayBlockBuffer::provideNextBlock()
{
    if (canRead)
    {
        size_t readIndex = readBlockIndex * DSP::blockSize;

        outputBufferL.copy(&bufferL[readIndex]);
        outputBufferR.copy(&bufferR[readIndex]);
    }
    else
    {
        outputBufferL.fill(0.0);
        outputBufferR.fill(0.0);
    }

    readBlockIndex++;

    if (readBlockIndex >= blockCount)
        readBlockIndex = 0;
}

// Return current buffer blocks
size_t DelayBlockBuffer::size() const
{
    return blockCount;
}
