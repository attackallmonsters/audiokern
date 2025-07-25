#pragma once

#include "DSP.h"
#include "dsp_types.h"
#include "DSPSampleBuffer.h"
#include "clamp.h"
#include <vector>

/**
 * @brief Block-based ring buffer for stereo sample delay and feedback handling.
 *
 * This class implements a circular buffer using full DSP blocks for left and right
 * channels. Time-based sizing is supported by converting milliseconds into
 * multiples of `DSP::blockSize`, which allows efficient use in delay effects,
 * comb filters, and reverb feedback paths.
 *
 * ### Usage Example
 * @code
 * RingBlockBuffer delayBuffer;
 * delayBuffer.setMaxTime(200);         // max. delay 200 ms
 * delayBuffer.setTime(75.0, 85.0);     // L = 75 ms, R = 85 ms delay
 * delayBuffer.initialize("delayA");    // allocate buffers
 *
 * while (running) {
 *     delayBuffer.push(inputL, inputR); // shift data in/out per block
 *     outputL.copy(delayBuffer.outputBufferL);
 *     outputR.copy(delayBuffer.outputBufferR);
 * }
 * @endcode
 *
 * @note Intern verwendet der Buffer interne Indexberechnung für links/rechts unabhängig.
 * @note Der Zugriff erfolgt ausschließlich blockweise über `push()` und Output-/Feedback-Puffer.
 */
class RingBlockBuffer
{
public:
    /**
     * @brief Default constructor. Buffers are uninitialized.
     */
    RingBlockBuffer();

    /**
     * @brief Returns the internal buffer name (set during initialization).
     * @return Reference to name string.
     */
    const std::string &getName() const { return bufferName; }

    /**
     * @brief Sets the maximum supported buffer length in milliseconds.
     *
     * Internally rounds up to the next full DSP block size.
     * Must be called before `initialize()`.
     *
     * @param timeMS Maximum buffer time (e.g. 250.0 ms).
     */
    void setMaxTime(host_float timeMS);

    /**
     * @brief Sets the desired delay time for left and right channels.
     *
     * Must not exceed the previously defined maximum via `setMaxTime()`.
     * Can be used dynamically before or after `initialize()`.
     *
     * @param timeMSL Left channel delay time in milliseconds.
     * @param timeMSR Right channel delay time in milliseconds.
     */
    void setTime(host_float timeMSL, host_float timeMSR);

    /**
     * @brief Allocates memory and resets all indices and buffers.
     *
     * Converts time settings into block-aligned sizes based on `DSP::sampleRate`.
     * Sets up internal ring buffers and prepares output and feedback buffers.
     *
     * @param name Identifier used for logging/debugging.
     */
    void initialize(std::string name);

    /**
     * @brief Pushes a new stereo sample block into the ring buffer.
     *
     * Writes one block (L/R) to the internal buffer and reads the corresponding
     * delayed block into `outputBufferL` / `outputBufferR`.
     *
     * @param blockL Left input block.
     * @param blockR Right input block.
     */
    void push(const DSPSampleBuffer &blockL, const DSPSampleBuffer &blockR);

    /**
     * @brief Clears all buffers and resets write positions.
     */
    void clear();

    DSPSampleBuffer outputBufferL;  ///< Delayed output block (left)
    DSPSampleBuffer outputBufferR;  ///< Delayed output block (right)

    DSPSampleBuffer feedbackBufferL; ///< Feedback tap block (left)
    DSPSampleBuffer feedbackBufferR; ///< Feedback tap block (right)

private:
    std::vector<host_float> bufferL; ///< Internal ring buffer for left channel
    std::vector<host_float> bufferR; ///< Internal ring buffer for right channel

    size_t bufferSize = 0;        ///< Total ring buffer size (in samples)
    size_t bufferSizeL = 0;       ///< Delay size for left
    size_t bufferSizeR = 0;       ///< Delay size for right
    size_t writeIndexL = 0;       ///< Write index for left
    size_t writeIndexR = 0;       ///< Write index for right
    size_t blockCountL = 0;       ///< Number of blocks for left delay
    size_t blockCountR = 0;       ///< Number of blocks for right delay

    host_float maxTime = 0.0;     ///< Maximum delay time in ms
    host_float delayTimeMsL = 0.0;///< Current delay for left
    host_float delayTimeMsR = 0.0;///< Current delay for right

    std::string bufferName;       ///< Identifier for debugging/logging
};
