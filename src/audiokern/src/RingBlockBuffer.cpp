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

    writeIndexL = writeIndexR = 0;
    setTime(1.0, 1.0);
}

// Set target buffer length in milliseconds (no allocation yet)
void RingBlockBuffer::setTime(dsp_float timeMSL, dsp_float timeMSR)
{
    delayTimeMsL = timeMSL;
    delayTimeMsR = timeMSR;

    size_t rawSizeL = static_cast<size_t>((delayTimeMsL / 1000.0) * DSP::sampleRate);
    size_t rawSizeR = static_cast<size_t>((delayTimeMsR / 1000.0) * DSP::sampleRate);

    if (rawSizeL < DSP::blockSize)
        rawSizeL = DSP::blockSize;

    if (rawSizeR < DSP::blockSize)
        rawSizeR = DSP::blockSize;

    bufferSizeL = ((rawSizeL + DSP::blockSize - 1) / DSP::blockSize) * DSP::blockSize;
    bufferSizeR = ((rawSizeR + DSP::blockSize - 1) / DSP::blockSize) * DSP::blockSize;

    if (bufferL.size() != bufferSizeL)
    {
        bufferL.resize(bufferSizeL);
        blockCountL = bufferSizeL / DSP::blockSize;
        writeIndexL = 0;
    }

    if (bufferR.size() != bufferSizeR)
    {
        bufferR.resize(bufferSizeR);
        blockCountR = bufferSizeR / DSP::blockSize;
        writeIndexR = 0;
    }
}

#include <stdexcept>
// Push a sample into the circular buffer
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