#pragma once

#include "dsp_types.h"
#include <cstddef>
#include <algorithm>
#include <cstring>
#include <cmath>

// This is a buffer that manages buffers used by the host.
// Set preprocessor/compiole flag HOST_USE_DOUBLE to use double
class DSPSampleBuffer {
public:
    DSPSampleBuffer();
    ~DSPSampleBuffer();

    // Initializes the buffer without allocating memory
    void initialize(size_t size);

    // Allocate internal buffer (no zeroing by default)
    void create(size_t size);

    // Use external buffer (shared, not owned)
    DSPSampleBuffer& operator=(host_float* externalBuffer);

    // Use buffer from another DSPSampleBuffer (shared pointer)
    DSPSampleBuffer& operator=(const DSPSampleBuffer& other);

    // Element access
    host_float& operator[](size_t index);
    const host_float& operator[](size_t index) const;

    // Fill buffer with a constant value
    void fill(host_float value);

    // Copy from raw host_float buffer (caller must ensure matching size)
    void copy(const host_float* source);

    // Copy from another DSPBuffer (caller must ensure matching size)
    void copy(const DSPSampleBuffer& other);

    host_float* data();
    const host_float* data() const;
    size_t size() const;

    // Gets the highest value in the buffer
    host_float getPeakValue() const;

private:
    host_float* buffer = nullptr;
    size_t bufferSize = 0;
    bool ownsBuffer = false;
};
