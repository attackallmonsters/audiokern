#pragma once

#include "dsp_types.h"
#include <cstddef>
#include <algorithm>
#include <cstring>
#include <cmath>

class DSPBuffer {
public:
    DSPBuffer();
    ~DSPBuffer();

    // Initializes the buffer without allocating memory
    void initialize(size_t size);

    // Allocate internal buffer (no zeroing by default)
    void create(size_t size);

    // Use external buffer (shared, not owned)
    DSPBuffer& operator=(dsp_float* externalBuffer);

    // Use buffer from another DSPBuffer (shared pointer)
    DSPBuffer& operator=(const DSPBuffer& other);

    // Element access
    dsp_float& operator[](size_t index);
    const dsp_float& operator[](size_t index) const;

    // Fill buffer with a constant value
    void fill(dsp_float value);

    // Copy from raw dsp_float buffer (caller must ensure matching size)
    void copy(const dsp_float* source);

    // Copy from another DSPBuffer (caller must ensure matching size)
    void copy(const DSPBuffer& other);

    // Returns a pointer to the buffer
    dsp_float* data();

    // Returns a const pointer to the buffer
    const dsp_float* data() const;

    // Returns the buffer size
    size_t size() const;

    // Gets the highest value in the buffer
    dsp_float getPeakValue() const;

private:
    dsp_float* buffer = nullptr;
    size_t bufferSize = 0;
    bool ownsBuffer = false;
};
