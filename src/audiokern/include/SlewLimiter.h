#pragma once

#include "DSP.h"
#include "DSPObject.h"
#include "dsp_types.h"

/**
 * @brief Slew limiter for smoothing control changes over time.
 *
 * The `SlewLimiter` gradually transitions from its current value to a target value
 * over a defined time interval. It's typically used to smooth abrupt parameter changes
 * such as frequency shifts, modulation depth, or panning.
 *
 * The smoothing duration is specified in milliseconds and is converted into sample counts
 * based on the current sample rate. The `process()` method must be called once per sample
 * to advance the transition.
 *
 * ### Usage Example
 * @code
 * SlewLimiter slew;
 * slew.initialize();             // Must be called before use
 * slew.setSlewTime(10.0);        // Set rise/fall time to 10 ms
 * slew.setTarget(1.0);           // Set desired value
 *
 * for (int i = 0; i < DSP::blockSize; ++i)
 * {
 *     float value = slew.process();  // Use current smoothed value
 *     modSignal[i] *= value;
 * }
 * @endcode
 *
 * @note The slew starts whenever `setTarget()` is called with a value different from `get()`.
 * @note If `slewTime` is 0.0, the target is reached instantly.
 */
class SlewLimiter : public DSPObject
{
public:
    /**
     * @brief Constructs a new SlewLimiter instance.
     *
     * Default slew time is 0 ms (i.e., immediate transition).
     */
    SlewLimiter();

    /**
     * @brief Sets a new target value.
     *
     * The transition to the new value begins immediately over the configured slew time.
     *
     * @param newTarget Target value to reach.
     */
    void setTarget(host_float newTarget);

    /**
     * @brief Sets the slew duration in milliseconds.
     *
     * @param ms Smoothing time for ramping to a new target.
     */
    void setSlewTime(host_float ms);

    /**
     * @brief Advances the internal state by one sample.
     *
     * Should be called once per sample to retrieve the current value.
     *
     * @return The current interpolated value.
     */
    host_float process();

    /**
     * @brief Process a complete block and return the final value
     *
     * Advances the slew limiter by DSP::blockSize samples and returns
     * the final calculated value after processing the entire block.
     *
     * @return Final smoothed value after processing the block
     */
    host_float processBlock();

    /**
     * @brief Restarts the current transition from scratch.
     *
     * Resets to current value and re-applies the step toward target.
     */
    void restart();

    /**
     * @brief Indicates whether the limiter has fully reached the target.
     *
     * @return True if no interpolation is in progress.
     */
    bool isIdle() { return idle; }

    /**
     * @brief Returns the current value.
     *
     * @return Current interpolated value (not necessarily equal to target).
     */
    double get() const { return current; }

protected:
    /**
     * @brief Initializes the SlewLimiter and calculates internal parameters.
     */
    void initializeObject() override;

private:
    /**
     * @brief Converts the current slew time (in ms) to sample-based steps.
     *
     * Called internally after `setSlewTime()` or during initialization.
     */
    void calcSamples();

    size_t samplerate = 44100; ///< Current sample rate
    size_t slewSamples = 0;    ///< Number of samples over which to interpolate
    size_t remaining = 0;      ///< Remaining samples until target is reached
    host_float current = 0.0;  ///< Current interpolated value
    host_float target = 0.0;   ///< Target value to reach
    host_float step = 0.0;     ///< Per-sample increment
    bool idle = true;          ///< True if no change is in progress
    host_float slewTime = 0.0; ///< Slew duration in milliseconds
};
