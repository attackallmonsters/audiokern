#pragma once

#include "SoundProcessor.h"
#include "DSPSampleBuffer.h"
#include "dsp_types.h"
#include "clamp.h"
#include <vector>
#include <cmath>
#include <algorithm>

/**
 * @brief Simple lookahead limiter with adjustable threshold, release time, and lookahead.
 *
 * The limiter detects signal peaks ahead of time using a circular lookahead buffer
 * and applies gain reduction when the signal exceeds the configured threshold.
 * A release coefficient is used to smooth gain recovery.
 *
 * Designed for block-based stereo audio processing.
 */
class Limiter : public SoundProcessor
{
public:
    /**
     * @brief Constructs an uninitialized limiter.
     *
     * You must call `initialize()` via the DSP framework to use it.
     */
    Limiter();

    /**
     * @brief Sets the threshold level at which limiting begins.
     *
     * @param thres Threshold in dBFS (range: -100.0 to 0.0).
     */
    void setThreshold(dsp_float thres);

    /**
     * @brief Sets the release time constant (smoothing after gain reduction).
     *
     * Higher values result in slower recovery.
     *
     * @param releaseMs Release time in milliseconds.
     */
    void setReleaseTime(dsp_float releaseMs);

    /**
     * @brief Sets the lookahead time used to anticipate peaks.
     *
     * This defines the length of the circular buffer used to delay output
     * so that peaks can be detected before they occur.
     *
     * @param ms Lookahead duration in milliseconds.
     */
    void setLookaheadTime(dsp_float ms);

    /**
     * @brief Resets the internal state (gain, buffer indices, etc.).
     */
    void reset();

protected:
    /**
     * @brief Initializes the processor and internal buffers.
     *
     * Called by the DSP system during registration.
     */
    void initializeProcessor() override;

private:
    /**
     * @brief Static entry point for block processing (used by DSPObject dispatcher).
     *
     * Applies limiting with lookahead to `processBufferL` and `processBufferR`.
     *
     * @param dsp Pointer to this limiter instance, cast from DSPObject.
     */
    static void processBlock(DSPObject *dsp);

    /**
     * @brief Internal implementation of the limiting algorithm.
     *
     * Uses peak detection, gain smoothing, and delayed output via circular buffer.
     */
    void processBlock();

    /**
     * @brief Lookahead buffer holding delayed stereo samples.
     *
     * Circular buffer containing (L,R) sample pairs.
     */
    std::vector<std::pair<host_float, host_float>> lookaheadBuffer;

    dsp_float threshold = 1.0;     ///< Linear amplitude threshold (derived from dB)
    dsp_float releaseCoeff = 0.99; ///< Smoothing coefficient for release
    int lookaheadSamples = 0;      ///< Size of lookahead buffer in samples
    int bufferIndex = 0;           ///< Current write position in circular buffer
    dsp_float gain = 1.0;          ///< Current gain reduction factor
};
