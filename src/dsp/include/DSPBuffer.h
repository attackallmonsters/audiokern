// DSPBuffer: A utility class for managing a dynamic buffer of dsp_float-precision audio samples.
// Provides common operations such as resize, clear, gain application, and deep copying.

#pragma once
#include <vector>
#include <cstddef>
#include "dsp_types.h"

class DSPBuffer
{
public:
    // Constructor with optional initial size (default: 2048 samples)
    explicit DSPBuffer();

    // Resize the internal buffer and initialize new elements to 0.0
    void resize(size_t newSize);

    // Set all buffer elements to 0.0
    void clear();

    // Return a mutable pointer to the internal buffer data
    dsp_float *data();

    // Return a const pointer to the internal buffer data
    const dsp_float *data() const;

    // Return the number of elements in the buffer
    size_t size() const;

    // Element access by index (read/write)
    dsp_float &operator[](size_t index);

    // Element access by index (read-only for const instances)
    const dsp_float &operator[](size_t index) const;

    // Multiply all buffer samples by a scalar gain value
    void applyGain(dsp_float gain);

    // Copy contents from another DSPBuffer instance
    void set(const DSPBuffer &other);

    // Copy raw data from an external float array into the buffer
    void set(const float *source);

#ifndef USE_SINGE_PRECISION
    // Copy raw data from an external dsp_float array into the buffer
    void set(const double *source);
#endif

    // Fill the buffer with a constant value
    void fill(dsp_float value);

    // Switches the current buffer to a source buffer.
    void switchTo(DSPBuffer &buf);

    // Restores the buffer reference previously changed with switchTo.
    void restore();

    // Create and return a deep copy of this buffer
    DSPBuffer clone() const;

private:
    std::vector<dsp_float> buffer;                // Internal buffer storage
    std::vector<dsp_float> *bufferOrig = nullptr; // Internal buffer storage
};
