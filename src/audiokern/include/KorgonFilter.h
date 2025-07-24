#pragma once

#include "SoundProcessor.h"
#include "DSPSampleBuffer.h"
#include "VoiceOptions.h"
#include "dsp_types.h"
#include "clamp.h"
#include "dsp_math.h"
#include <cmath>

/**
 * @brief KorgonFilter is an analog-inspired dual-integrator filter with nonlinear feedback.
 *
 * This filter implements a state variable topology using two cascaded integrators per channel.
 * It includes a diode-style nonlinearity in the feedback path to produce saturation and soft distortion.
 * The filter supports lowpass and highpass modes and exposes resonance and drive as parameters.
 */
class KorgonFilter : public SoundProcessor
{
public:
    /**
     * @brief Constructs a KorgonFilter instance.
     *
     * Initializes all internal state and registers the default processing callback.
     */
    explicit KorgonFilter();

    /**
     * @brief Sets the drive amount before feedback stage.
     *
     * A higher drive results in stronger saturation and nonlinear behavior.
     *
     * @param value Drive gain multiplier, typical range: 0.1 – 2.0
     */
    void setDrive(host_float value);

    /**
     * @brief Selects the filter mode.
     *
     * Supported modes: FilterMode::LP (lowpass), FilterMode::HP (highpass)
     *
     * @param mode Filter mode to use
     */
    void setFilterMode(FilterMode mode);

    /**
     * @brief Sets the resonance amount.
     *
     * Resonance controls the amount of feedback in the filter path.
     *
     * @param reso Resonance value, typically in the range 0.0 – 1.0
     */
    void setResonance(host_float reso);

    /**
     * @brief Resets all filter integrators and state variables to zero.
     */
    void reset();

protected:
    /**
     * @brief Initializes the processor, including state and bus connections.
     *
     * This is called once after block size and sample rate are known.
     */
    void initializeProcessor() override;

private:
    // === DSP Processing ===

    /**
     * @brief Static DSP callback for lowpass mode.
     * Dispatches to the instance method.
     */
    static void processBlockLP(DSPObject *dsp);

    /**
     * @brief Static DSP callback for highpass mode.
     * Dispatches to the instance method.
     */
    static void processBlockHP(DSPObject *dsp);

    /**
     * @brief Instance method for processing lowpass mode.
     */
    void processBlockLP();

    /**
     * @brief Instance method for processing highpass mode.
     */
    void processBlockHP();

    // === Filter State ===

    host_float y1L; ///< Output of first integrator (left channel)
    host_float y2L; ///< Output of second integrator = filter output (left)
    host_float y1R; ///< Output of first integrator (right channel)
    host_float y2R; ///< Output of second integrator = filter output (right)

    host_float T;      ///< Filter integration factor, typically based on sample rate
    host_float drive;  ///< Pre-gain for nonlinear feedback stage
    host_float resonance; ///< Feedback gain, controls resonance amount
    FilterMode filterMode; ///< Selected filter mode: LP or HP

    /**
     * @brief Nonlinear feedback function (e.g. tanh-like saturation).
     *
     * Simulates analog saturation behavior caused by diodes or transistors in the feedback path.
     *
     * @param s Input signal
     * @return Saturated output
     */
    static host_float nonlinearFeedback(host_float s);
};
