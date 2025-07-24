#include "DSPBuffer.h"

DSPBuffer::DSPBuffer() = default;

DSPBuffer::~DSPBuffer()
{
    if (ownsBuffer && buffer)
        delete[] buffer;
}

void DSPBuffer::initialize(size_t size)
{
    buffer = nullptr;
    bufferSize = size;
    ownsBuffer = false;
}

void DSPBuffer::create(size_t size)
{
    if (ownsBuffer && buffer)
        delete[] buffer;

    buffer = new dsp_float[size];
    bufferSize = size;
    ownsBuffer = true;

    fill(0.0);
}

DSPBuffer &DSPBuffer::operator=(dsp_float *externalBuffer)
{
    if (ownsBuffer && buffer)
        delete[] buffer;

    buffer = externalBuffer;
    ownsBuffer = false;
    return *this;
}

DSPBuffer &DSPBuffer::operator=(const DSPBuffer &other)
{
    if (ownsBuffer && buffer)
        delete[] buffer;

    buffer = other.buffer;
    bufferSize = other.bufferSize;
    ownsBuffer = false;
    return *this;
}

dsp_float &DSPBuffer::operator[](size_t index)
{
    return buffer[index];
}

const dsp_float &DSPBuffer::operator[](size_t index) const
{
    return buffer[index];
}

void DSPBuffer::fill(dsp_float value)
{
    std::fill(buffer, buffer + bufferSize, value);
}

void DSPBuffer::copy(const dsp_float *source)
{
    std::memcpy(buffer, source, bufferSize * sizeof(dsp_float));
}

void DSPBuffer::copy(const DSPBuffer &other)
{
    std::memcpy(buffer, other.buffer, bufferSize * sizeof(dsp_float));
}

dsp_float *DSPBuffer::data()
{
    return buffer;
}

const dsp_float *DSPBuffer::data() const
{
    return buffer;
}

size_t DSPBuffer::size() const
{
    return bufferSize;
}

dsp_float DSPBuffer::getPeakValue() const
{
    dsp_float peak = 0.0;

    for (size_t i = 0; i < bufferSize; ++i)
    {
        dsp_float absVal = std::fabs(buffer[i]);

        if (absVal > peak)
            peak = absVal;
    }
    return peak;
}