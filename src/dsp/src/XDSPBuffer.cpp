// === XDSPBuffer.cpp ===
#include "DSP.h"
#include "XDSPBuffer.h"
#include <algorithm>
#include "dsp_types.h"
#include "clamp.h"

// Constructor with optional initial size (default: 2048 samples)
XDSPBuffer::XDSPBuffer()
{
    buffer.resize(DSP::maxBlockSize, 0.0);
    bufferOrig = &buffer;
}

// Resize the internal buffer and initialize new elements to 0.0
void XDSPBuffer::resize(size_t newSize)
{
    buffer.resize(clampmin(newSize , static_cast<size_t>(1)), 0.0);
    bufferOrig = &buffer;
}

// Set all buffer elements to 0.0
void XDSPBuffer::clear()
{
    std::fill(buffer.begin(), buffer.begin() + DSP::blockSize, 0.0);
}

// Return a mutable pointer to the internal buffer data
dsp_float *XDSPBuffer::data()
{
    return buffer.data();
}

// Return a const pointer to the internal buffer data
const dsp_float *XDSPBuffer::data() const
{
    return buffer.data();
}

// Return the number of elements in the buffer
size_t XDSPBuffer::size() const
{
    return buffer.size();
}

// Element access by index (read/write)
dsp_float &XDSPBuffer::operator[](size_t index)
{
    return buffer[index];
}

// Element access by index (read-only for const instances)
const dsp_float &XDSPBuffer::operator[](size_t index) const
{
    return buffer[index];
}

// Multiply all buffer samples by a scalar gain value
void XDSPBuffer::applyGain(dsp_float gain)
{
    for (auto &sample : buffer)
        sample *= gain;
}

// Copy contents from another XDSPBuffer instance
void XDSPBuffer::set(const XDSPBuffer &other)
{
    std::copy(other.buffer.begin(), other.buffer.begin() + DSP::blockSize, buffer.begin());
}

// Copy raw data from an external float array into the buffer
void XDSPBuffer::set(const float *source)
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
        buffer[i] = static_cast<dsp_float>(source[i]);
}

#ifndef USE_SINGLE_PRECISION
// Copy raw data from an external dsp_float array into the buffer
void XDSPBuffer::set(const double *source)
{
    std::copy(source, source + DSP::blockSize, buffer.begin());
}
#endif

// Fill the buffer with a constant value
void XDSPBuffer::fill(dsp_float value)
{
    std::fill(buffer.begin(), buffer.begin() + DSP::blockSize, value);
}

// Switches the current buffer to a source buffer (shallow reference switch)
void XDSPBuffer::switchTo(XDSPBuffer &buf)
{
    buffer = buf.buffer; // set external buffer to current buffer
}

// Restores the buffer reference previously changed with switchTo
void XDSPBuffer::restore()
{
    buffer = std::move(*bufferOrig); // Restore saved buffer
}

// Create and return a deep copy of this buffer
XDSPBuffer XDSPBuffer::clone() const
{
    XDSPBuffer clonedCopy;
    std::copy(buffer.begin(), buffer.begin() + DSP::blockSize, clonedCopy.buffer.begin());
    return clonedCopy;
}
