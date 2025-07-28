#pragma once

#include "dsp_types.h"
#include "clamp.h"
#include <vector>
#include <cstddef>
#include <stdexcept>

/**
 * @brief Simple FIFO (First-In-First-Out) ring buffer for host_float samples.
 *
 * This class implements a circular buffer with overwrite behavior when full.
 * It is suitable for storing a sliding window of recent samples, such as for:
 * - DC offset tracking
 * - RMS/peak analysis
 * - Smoothing or envelope following
 * - Simple delay lines (non-interpolated)
 *
 * Oldest entries are overwritten when the buffer is full.
 */
class FifoSampleBuffer
{
public:
    /**
     * @brief Default constructor (empty buffer).
     *
     * You must call `create(size)` before using the buffer.
     */
    FifoSampleBuffer();

    /**
     * @brief Allocates internal storage and resets the buffer.
     *
     * Clears any existing data and initializes a new buffer of given size.
     *
     * @param size Maximum number of elements to store.
     */
    void create(size_t size);

    /**
     * @brief Pushes a new sample into the buffer.
     *
     * If the buffer is full, the oldest value is overwritten.
     *
     * @param value New sample value to add.
     */
    void push(const host_float &value);

    /**
     * @brief Accesses an element relative to the oldest value.
     *
     * Index 0 corresponds to the oldest (first) sample still in the buffer.
     * Throws if index is out of bounds (i >= count()).
     *
     * @param i Index relative to the oldest sample (0 = oldest).
     * @return Reference to the sample (modifiable).
     */
    host_float &operator[](size_t i);

    /**
     * @brief Read-only version of sample access.
     *
     * @param i Index relative to the oldest sample (0 = oldest).
     * @return Sample value at the given position.
     */
    host_float operator[](size_t i) const;

    /**
     * @brief Returns the number of currently stored samples.
     *
     * Will be ≤ `size()`. If the buffer is full, returns `size()`.
     *
     * @return Number of valid samples.
     */
    size_t count() const;

    /**
     * @brief Returns the total capacity of the buffer.
     *
     * @return Allocated buffer size.
     */
    size_t size() const;

    /**
     * @brief Checks whether the buffer is currently empty.
     *
     * @return True if no valid elements are stored.
     */
    bool isEmpty() const;

    /**
     * @brief Checks whether the buffer is full.
     *
     * @return True if buffer holds `size()` elements.
     */
    bool isFull() const;

    /**
     * @brief Computes the sum of all values in the buffer.
     *
     * Useful for computing running averages or power.
     *
     * @return Sum of all current sample values.
     */
    host_float getSum();

    /**
     * @brief Clears the buffer contents.
     *
     * Resets internal state. Capacity remains unchanged.
     */
    void clear();

private:
    std::vector<host_float> buffer; ///< Internal storage array
    size_t bufferSize = 0;          ///< Maximum number of elements
    size_t writeIndex = 0;          ///< Next write position
    size_t readIndex = 0;           ///< Oldest value position
    size_t elements = 0;            ///< Number of currently valid elements
};
