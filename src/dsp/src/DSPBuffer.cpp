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

    buffer = new float[size];
    bufferSize = size;
    ownsBuffer = true;
}

DSPBuffer &DSPBuffer::operator=(float *externalBuffer)
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

float &DSPBuffer::operator[](size_t index)
{
    return buffer[index];
}

const float &DSPBuffer::operator[](size_t index) const
{
    return buffer[index];
}

void DSPBuffer::fill(float value)
{
    std::fill(buffer, buffer + bufferSize, value);
}

void DSPBuffer::copy(const float *source)
{
    std::memcpy(buffer, source, bufferSize * sizeof(float));
}

void DSPBuffer::copy(const DSPBuffer &other)
{
    std::memcpy(buffer, other.buffer, bufferSize * sizeof(float));
}

float *DSPBuffer::data()
{
    return buffer;
}

const float *DSPBuffer::data() const
{
    return buffer;
}

size_t DSPBuffer::size() const
{
    return bufferSize;
}
