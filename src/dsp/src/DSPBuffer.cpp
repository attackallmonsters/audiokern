// === DSPBuffer.cpp ===
#include "DSP.h"
#include "DSPBuffer.h"
#include <algorithm>
#include "dsp_types.h"
#include "clamp.h"

// Constructor with optional initial size (default: 2048 samples)
DSPBuffer::DSPBuffer()
{
    buffer.resize(DSP::maxBlockSize, 0.0);
    bufferOrig = &buffer;
}

// Resize the internal buffer and initialize new elements to 0.0
void DSPBuffer::resize(size_t newSize)
{
    buffer.resize(clampmin(newSize , static_cast<size_t>(1)), 0.0);
    bufferOrig = &buffer;
}

// Set all buffer elements to 0.0
void DSPBuffer::clear()
{
    std::fill(buffer.begin(), buffer.begin() + DSP::blockSize, 0.0);
}

// Return a mutable pointer to the internal buffer data
dsp_float *DSPBuffer::data()
{
    return buffer.data();
}

// Return a const pointer to the internal buffer data
const dsp_float *DSPBuffer::data() const
{
    return buffer.data();
}

// Return the number of elements in the buffer
size_t DSPBuffer::size() const
{
    return buffer.size();
}

// Element access by index (read/write)
dsp_float &DSPBuffer::operator[](size_t index)
{
    return buffer[index];
}

// Element access by index (read-only for const instances)
const dsp_float &DSPBuffer::operator[](size_t index) const
{
    return buffer[index];
}

// Multiply all buffer samples by a scalar gain value
void DSPBuffer::applyGain(dsp_float gain)
{
    for (auto &sample : buffer)
        sample *= gain;
}

// Copy contents from another DSPBuffer instance
void DSPBuffer::set(const DSPBuffer &other)
{
    std::copy(other.buffer.begin(), other.buffer.begin() + DSP::blockSize, buffer.begin());
}

// Copy raw data from an external float array into the buffer
void DSPBuffer::set(const float *source)
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
        buffer[i] = static_cast<dsp_float>(source[i]);
}

#ifndef USE_SINGLE_PRECISION
// Copy raw data from an external dsp_float array into the buffer
void DSPBuffer::set(const double *source)
{
    std::copy(source, source + DSP::blockSize, buffer.begin());
}
#endif

// Fill the buffer with a constant value
void DSPBuffer::fill(dsp_float value)
{
    std::fill(buffer.begin(), buffer.begin() + DSP::blockSize, value);
}

// Switches the current buffer to a source buffer (shallow reference switch)
void DSPBuffer::switchTo(DSPBuffer &buf)
{
    buffer = buf.buffer; // set external buffer to current buffer
}

// Restores the buffer reference previously changed with switchTo
void DSPBuffer::restore()
{
    buffer = std::move(*bufferOrig); // Restore saved buffer
}

// Create and return a deep copy of this buffer
DSPBuffer DSPBuffer::clone() const
{
    DSPBuffer clonedCopy;
    std::copy(buffer.begin(), buffer.begin() + DSP::blockSize, clonedCopy.buffer.begin());
    return clonedCopy;
}
