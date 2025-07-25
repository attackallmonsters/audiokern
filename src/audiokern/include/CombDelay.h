#pragma once

#include "SoundEffect.h"
#include "RingBlockBuffer.h"
#include "DSPSampleBuffer.h"
#include "ParamFader.h"
#include "dsp_math.h"
#include "clamp.h"
#include <vector>

/**
 * @brief A stereo comb filter with variable delay, feedback and damping.
 *
 * The CombDelay object applies a delay effect where the input signal is fed back into itself
 * after a short delay time. Optionally, a damping lowpass filter can be applied to the feedback path.
 * This is a common building block in reverb and chorus effects.
 *
 * Delay times and feedback amounts are set in milliseconds and normalized units respectively.
 */
class CombDelay : public SoundEffect
{
public:
    /**
     * @brief Constructs a new CombDelay instance.
     *
     * Initializes internal parameters and connects to standard I/O busses.
     */
    explicit CombDelay();

    /**
     * @brief Sets the maximum allowed delay time in milliseconds.
     *
     * This defines the internal buffer size and must be called before using setTime().
     * Should be called during initialization only.
     *
     * @param timeMS Maximum delay time in milliseconds.
     */
    void setMaxTime(host_float timeMS);

    /**
     * @brief Sets the current delay time.
     *
     * The actual applied delay will be clamped to the range defined by setMaxTime().
     *
     * @param timeMS Delay time in milliseconds.
     */
    void setTime(host_float timeMS);

    /**
     * @brief Sets a time ratio used to scale the base delay time of the comb filter.
     *
     * This ratio is used to scale internal delay times. Applying different ratios can help to avoid
     * resonances, increase stereo width, and introduce natural-sounding temporal variation.
     *
     * @param ratio The time ratio to apply to the base delay time.
     */
    void setTimeRatio(dsp_math::TimeRatio ratio);

    /**
     * @brief Sets the feedback amount.
     *
     * The value defines how much of the delayed signal is re-injected into the input.
     * Typical values range from 0.0 (no feedback) to 0.99 (strong feedback).
     *
     * @param fb Feedback amount (0.0 – 1.0).
     */
    void setFeedback(host_float fb);

    /**
     * @brief Sets the damping filter frequency applied to the feedback signal.
     *
     * Acts as a one-pole lowpass filter to reduce high-frequency buildup in the feedback loop.
     *
     * @param freqHz Damping filter cutoff frequency in Hz.
     */
    void setDamping(host_float freqHz);

    /**
     * @brief Pushes a stereo input block into the delay buffer and writes the output to the output bus.
     *
     * This is used internally in the DSP loop. Feedback is applied using the internal feedback gain
     * and damping is performed per channel before reinjection.
     */
    void push();

protected:
    /**
     * @brief Initializes the effect.
     *
     * Connects to standard input/output/modulation busses, initializes internal buffers and parameters.
     */
    void initializeEffect() override;

    /**
     * @brief Called when the output bus is successfully connected.
     *
     * Used to initialize or reset internal buffers.
     */
    void onOutputBusConnected() override;

private:
    /**
     * @brief Static DSP callback wrapper.
     *
     * Called by the DSP system for block-based processing.
     *
     * @param dsp Pointer to the CombDelay instance (cast from DSPObject*).
     */
    static void processBlock(DSPObject *dsp);

    /**
     * @brief Main block processing method.
     *
     * Reads input from the connected input bus, applies comb filtering with feedback and damping,
     * and writes the result to the output bus.
     */
    void processBlock();

    /// The current delay time
    host_float currentTime;

    /// CombDelay time relations L/R
    dsp_math::TimeRatio timeRatio;

    /// Feedback amount [0.0 – 1.0]
    host_float feedback = 0.5;

    /// Damping filter coefficient [0.0 – 1.0]
    host_float dampingCoeff = 0.2;

    /// State for left channel damping filter
    host_float dampingStateL = 0.0;

    /// State for right channel damping filter
    host_float dampingStateR = 0.0;

    /// Internal ring buffer for stereo block-based delay
    RingBlockBuffer delayBuffer;

    /// Parameter fade handler for smooth transitions during time/feedback/damping changes
    ParamFader paramFader;
};
