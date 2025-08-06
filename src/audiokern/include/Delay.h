#pragma once

#include "SoundEffect.h"
#include "RingBlockBuffer.h"
#include "DSPSampleBuffer.h"
#include "ParamFader.h"
#include "dsp_math.h"
#include "clamp.h"
#include "CrossFader.h"
#include <vector>

#pragma once
#include <cmath>



/**
 * @brief Stereo delay effect with configurable delay time and feedback.
 *
 * This class implements a standard stereo delay using a ring buffer structure.
 * Feedback can be set per channel. Delay time is adjustable up to a defined maximum.
 * Audio is processed blockwise using the DSP framework. Use setMaxTime first to
 * initialize the maximum delay time.
 */
class Delay : public SoundEffect
{
public:
    /**
     * @brief Constructs a new Delay instance.
     */
    explicit Delay();

    /**
     * @brief Sets the maximum delay time in milliseconds.
     *
     * Must be called before setting the actual delay time. Allocates buffer capacity.
     *
     * @param timeMS Maximum delay time (in ms)
     */
    void setMaxTime(host_float timeMS);

    /**
     * @brief Sets the actual delay time for each channel (in milliseconds).
     *
     * The delay times will be clamped to the previously set maximum time.
     *
     * @param timeMSL Delay time for left channel (ms)
     * @param timeMSR Delay time for right channel (ms)
     */
    void setTime(host_float timeMSL, host_float timeMSR);

    /**
     * @brief Calculates the right delay time based on a given left delay time and musical or psychoacoustic ratio.
     *
     * @param ratio The stereo timing ratio to apply.
     * @return double Delay time on the right channel in seconds.
     */
    void setTimeRatio(dsp_math::TimeRatio ratio);

    /**
     * @brief Sets the feedback amount per channel.
     *
     * Values should typically remain below 1.0 to avoid instability.
     *
     * @param fbL Feedback amount for left channel (0.0 to <1.0)
     * @param fbR Feedback amount for right channel (0.0 to <1.0)
     */
    void setFeedback(host_float fbL, host_float fbR);

    /**
     * @brief Ring buffer that holds internal delay lines.
     */
    RingBlockBuffer delayBuffer;

protected:
    /**
     * @brief Initializes the delay effect, allocates buffers and resets state.
     */
    void initializeEffect() override;

    /**
     * @brief Called when the output bus is connected.
     *
     * Used to synchronize the delay buffer state with output.
     */
    void onOutputBusConnected(DSPAudioBus &bus) override;

private:
    /**
     * @brief Static DSP entry point.
     *
     * Routes processing to the instance method.
     */
    static void processBlock(DSPObject *dsp);

    /**
     * @brief Processes one audio block: applies delay and feedback.
     */
    void processBlock();

    /// @brief current delay times
    host_float currentTimeL, currentTimeR;

    /// @brief Feedback levels for each channel
    host_float feedbackL = 0.0;
    host_float feedbackR = 0.0;

    /// @brief  Time ration left => right
    dsp_math::TimeRatio timeRatio;

    /// @brief Internal parameter fader for smooth transitions
    ParamFader paramFader;
};
