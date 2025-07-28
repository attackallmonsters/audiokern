#include "DSPSampleBuffer.h"

DSPSampleBuffer::DSPSampleBuffer()
{
    buffer = nullptr;
    ownsBuffer = false;
}

DSPSampleBuffer::~DSPSampleBuffer()
{
    if (ownsBuffer && buffer)
        delete[] buffer;
}

void DSPSampleBuffer::initialize(const std::string &name, size_t size)
{
    if (ownsBuffer && buffer)
        delete[] buffer;

    buffer = new host_float[size];
    bufferSize = size;
    ownsBuffer = true;
    bufferName = name;

    fill(0.0);
}

DSPSampleBuffer &DSPSampleBuffer::operator=(host_float *externalBuffer)
{
    if (ownsBuffer && buffer)
        delete[] buffer;

    buffer = externalBuffer;
    ownsBuffer = false;
    bufferName = "_external";
    return *this;
}

DSPSampleBuffer &DSPSampleBuffer::operator=(const DSPSampleBuffer &other)
{
    if (ownsBuffer && buffer)
        delete[] buffer;

    buffer = other.buffer;
    bufferSize = other.bufferSize;
    ownsBuffer = false;
    bufferName = other.bufferName;
    return *this;
}

void DSPSampleBuffer::assign(const std::string &name, host_float *externalBuffer)
{
    if (ownsBuffer && buffer)
        delete[] buffer;

    buffer = externalBuffer;
    ownsBuffer = false;
    bufferName = name;
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

host_float DSPSampleBuffer::peak() const
{
    host_float peak = 0.0;

    for (size_t i = 0; i < bufferSize; ++i)
    {
        host_float absVal = std::fabs(buffer[i]);

        if (absVal > peak)
            peak = absVal;
    }
    return peak;
}

void DSPSampleBuffer::free()
{
    if (ownsBuffer && buffer)
        delete[] buffer;
}

void DSPSampleBuffer::isValid()
{
    constexpr host_float maxReasonable = 1.0e6;

    for (size_t i = 0; i < size(); ++i)
    {
        host_float value = buffer[i];
        if (std::isnan(value))
        {
            log();
            PANIC("-nan in " + bufferName + " at position " + std::to_string(i));
        }

        if (std::isinf(value))
        {
            log();
            PANIC("-inf in " + bufferName + " at position " + std::to_string(i));
        }

        if (std::fabs(value) > maxReasonable)
        {
            log();
            PANIC("Value too large (" + std::to_string(value) + ") in " + bufferName + " at index " + std::to_string(i));
        }
    }
}

void DSPSampleBuffer::log()
{
    DSP::logBuffer(bufferName, buffer, bufferSize);
}