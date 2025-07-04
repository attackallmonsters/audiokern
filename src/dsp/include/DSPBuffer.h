#pragma once

#include "dsp_types.h"
#include <cstddef>
#include <algorithm>
#include <cstring>

class DSPBuffer {
public:
    DSPBuffer();
    ~DSPBuffer();

    // Initializes the buffer without allocating memory
    void initialize(size_t size);

    // Allocate internal buffer (no zeroing by default)
    void create(size_t size);

    // Use external buffer (shared, not owned)
    DSPBuffer& operator=(float* externalBuffer);

    // Use buffer from another DSPBuffer (shared pointer)
    DSPBuffer& operator=(const DSPBuffer& other);

    // Element access
    float& operator[](size_t index);
    const float& operator[](size_t index) const;

    // Fill buffer with a constant value
    void fill(float value);

    // Copy from raw float buffer (caller must ensure matching size)
    void copy(const float* source);

    // Copy from another DSPBuffer (caller must ensure matching size)
    void copy(const DSPBuffer& other);

    float* data();
    const float* data() const;
    size_t size() const;

private:
    float* buffer = nullptr;
    size_t bufferSize = 0;
    bool ownsBuffer = false;
};
