#pragma once

#include "dsp_types.h"
#include "clamp.h"
#include <vector>
#include <cstddef>
#include <stdexcept>

// A simple generic ring buffer (circular buffer) with FIFO behavior
// Stores up to 'bufferSize' elements, overwriting the oldest when full

class FifoSampleBuffer
{
public:
    FifoSampleBuffer(); // Default constructor

    // Allocate internal storage with given capacity and reset state
    void create(size_t size);

    // Add a new value to the buffer, overwriting the oldest if full
    void push(const host_float &value);

    // Access an element by index relative to the oldest value (0 = oldest)
    host_float &operator[](size_t i);

    // Const version of operator[] for read-only access
    host_float operator[](size_t i) const;

    // Return current number of stored elements (up to capacity)
    size_t count() const;

    // Return the overall buffer size
    size_t size() const;

    // True if buffer currently holds no elements
    bool isEmpty() const;

    // True if buffer is full and oldest entries are overwritten
    bool isFull() const;

    // Gets a sum of all values in the buffer
    host_float getSum();

    // Clear all stored elements (but keep allocated capacity)
    void clear();

private:
    std::vector<host_float> buffer; // Internal storage
    size_t bufferSize = 0;          // Max number of elements
    size_t writeIndex = 0;          // Index for next write
    size_t readIndex = 0;           // Index of oldest element
    size_t elements = 0;               // Number of valid elements
};