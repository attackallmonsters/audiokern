#include "DSPSampleBuffer.h"

DSPSampleBuffer::DSPSampleBuffer() = default;

DSPSampleBuffer::~DSPSampleBuffer()
{
    if (ownsBuffer && buffer)
        delete[] buffer;
}

void DSPSampleBuffer::initialize(size_t size)
{
    buffer = nullptr;
    bufferSize = size;
    ownsBuffer = false;
}

void DSPSampleBuffer::create(size_t size)
{
    if (ownsBuffer && buffer)
        delete[] buffer;

    buffer = new host_float[size];
    bufferSize = size;
    ownsBuffer = true;
}

DSPSampleBuffer &DSPSampleBuffer::operator=(host_float *externalBuffer)
{
    if (ownsBuffer && buffer)
        delete[] buffer;

    buffer = externalBuffer;
    ownsBuffer = false;
    return *this;
}

DSPSampleBuffer &DSPSampleBuffer::operator=(const DSPSampleBuffer &other)
{
    if (ownsBuffer && buffer)
        delete[] buffer;

    buffer = other.buffer;
    bufferSize = other.bufferSize;
    ownsBuffer = false;
    return *this;
}

host_float &DSPSampleBuffer::operator[](size_t index)
{
    return buffer[index];
}

const host_float &DSPSampleBuffer::operator[](size_t index) const
{
    return buffer[index];
}

void DSPSampleBuffer::fill(host_float value)
{
    std::fill(buffer, buffer + bufferSize, value);
}

void DSPSampleBuffer::copy(const host_float *source)
{
    std::memcpy(buffer, source, bufferSize * sizeof(host_float));
}

void DSPSampleBuffer::copy(const DSPSampleBuffer &other)
{
    std::memcpy(buffer, other.buffer, bufferSize * sizeof(host_float));
}

host_float *DSPSampleBuffer::data()
{
    return buffer;
}

const host_float *DSPSampleBuffer::data() const
{
    return buffer;
}

size_t DSPSampleBuffer::size() const
{
    return bufferSize;
}
