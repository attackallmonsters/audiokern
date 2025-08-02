#pragma once

#include "DSP.h"
#include "dsp_types.h"
#include "omfg.h"
#include <cstddef>
#include <algorithm>
#include <string>
#include <cstring>
#include <cmath>

/**
 * @brief Represents a sample buffer used for audio signal processing.
 *
 * This class manages a 1D buffer of `host_float` values (either `float` or `double`, depending on `HOST_USE_DOUBLE`).
 * The buffer can either own its memory or refer to external/shared memory.
 */
class DSPSampleBuffer
{
public:
    /**
     * @brief Constructs an empty, uninitialized buffer.
     */
    DSPSampleBuffer();

    /**
     * @brief Destroys the buffer and releases owned memory.
     */
    ~DSPSampleBuffer();

    /**
     * @brief Returns the name assigned to this buffer.
     * @return Reference to the buffer name string.
     */
    const std::string &getName() const { return bufferName; }

    /**
     * @brief Initializes the buffer with a given name and size, allocating memory.
     * @param name Name of the buffer.
     * @param size Number of samples.
     */
    void initialize(const std::string &name, size_t size);

    /**
     * @brief Assigns an external raw buffer to this object (no memory ownership).
     * @param externalBuffer Pointer to external host_float buffer.
     * @return Reference to this buffer.
     */
    DSPSampleBuffer &operator=(host_float *externalBuffer);

    /**
     * @brief Shares an existing buffer from another DSPSampleBuffer (no copy).
     * @param other Source buffer to link to.
     * @return Reference to this buffer.
     */
    DSPSampleBuffer &operator=(const DSPSampleBuffer &other);

    /**
     * @brief Accesses a buffer element for read/write.
     * @param index Sample index.
     * @return Reference to the sample.
     */
    host_float &operator[](size_t index);

    /**
     * @brief Accesses a buffer element for read-only.
     * @param index Sample index.
     * @return Const reference to the sample.
     */
    const host_float &operator[](size_t index) const;

    /**
     * @brief Assigns an external buffer and name.
     * @param name Name to assign.
     * @param externalBuffer External sample buffer (not owned).
     */
    void assign(const std::string &name, host_float *externalBuffer);

    /**
     * @brief Fills the buffer with a constant value.
     * @param value Value to fill.
     */
    void fill(host_float value);

    /**
     * @brief Copies values from a raw buffer. Caller must ensure the size matches.
     * @param source Source buffer pointer.
     */
    void copy(const host_float *source);

    /**
     * @brief Copies values from another DSPSampleBuffer. Caller must ensure the size matches.
     * @param other Source buffer.
     */
    void copy(const DSPSampleBuffer &other);

    /**
     * @brief Returns a pointer to the internal buffer data.
     * @return Pointer to sample data.
     */
    host_float *data();

    /**
     * @brief Returns a const pointer to the internal buffer data.
     * @return Const pointer to sample data.
     */
    const host_float *data() const;

    /**
     * @brief Returns the number of samples in the buffer.
     * @return Sample count.
     */
    size_t size() const;

    /**
     * @brief Returns the peak (maximum absolute) value in the buffer.
     * @return Maximum absolute sample value.
     */
    host_float peak() const;

    /**
     * @brief Frees allocated memory if owned.
     */
    void free();

    void multiplyWith(DSPSampleBuffer &targetBuffer);

    /**
     * @brief Validates buffer content (e.g. check for NaNs or denormals).
     *
     * Used for debugging or runtime validation.
     */
    void isValid();

    /**
     * @brief Logs buffer content via DSP::log, including name and values.
     */
    void log();

private:
    host_float *buffer = nullptr; ///< Pointer to buffer memory
    size_t bufferSize = 0;        ///< Number of samples
    bool ownsBuffer = false;      ///< Whether this instance owns the memory
    std::string bufferName;       ///< Human.meadable name for logging/debugging
};
