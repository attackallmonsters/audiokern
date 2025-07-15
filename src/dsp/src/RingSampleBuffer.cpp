#include "RingSampleBuffer.h"

RingSampleBuffer::RingSampleBuffer()
{
}

void RingSampleBuffer::create(size_t size)
{
    bufferSize = clampmin(size, static_cast<size_t>(0));
    buffer.resize(bufferSize);
    clear();
}

void RingSampleBuffer::push(const host_float &value)
{
    buffer[writeIndex] = value;
    writeIndex = (writeIndex + 1) % bufferSize;

    if (elements < bufferSize)
        ++elements;
    else
        readIndex = (readIndex + 1) % bufferSize;
}

host_float &RingSampleBuffer::operator[](size_t i)
{
    return buffer[(readIndex + i) % bufferSize];
}

host_float RingSampleBuffer::operator[](size_t i) const
{
    return buffer[(readIndex + i) % bufferSize];
}

size_t RingSampleBuffer::count() const
{
    return elements;
}

size_t RingSampleBuffer::size() const
{
    return bufferSize;
}

bool RingSampleBuffer::isEmpty() const
{
    return elements == 0;
}

bool RingSampleBuffer::isFull() const
{
    return elements == bufferSize;
}

host_float RingSampleBuffer::getSum()
{
    host_float sum = 0.0;

    for (size_t i = 0; i < bufferSize; ++i)
        sum += buffer[i];

    return sum;
}

void RingSampleBuffer::clear()
{
    writeIndex = 0;
    readIndex = 0;
    elements = 0;
}