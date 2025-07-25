#pragma once

#include "dsp_types.h"

/**
 * @brief Simple one-pole DC blocking filter.
 *
 * This high-pass filter removes DC offset by subtracting the previous input and applying
 * a leaky integrator. It's useful for cleaning up signals before amplification or modulation.
 *
 * Transfer function (difference equation):
 * y[n] = x[n] - x[n-1] + R * y[n-1]
 *
 * where `R` is a feedback coefficient (typically close to 1.0).
 */
class DCBlocker
{
public:
    /**
     * @brief Constructs a DCBlocker with a given feedback coefficient.
     * @param r Feedback coefficient `R` in range (0.0, 1.0). Defaults to 0.995.
     */
    DCBlocker(dsp_float r = 0.995);

    /**
     * @brief Resets the internal filter state (clears history).
     */
    void reset();

    /**
     * @brief Sets the feedback coefficient `R`, which controls the cutoff frequency.
     * @param r Value between 0.0 and 1.0 (exclusive). Higher values allow lower cutoff.
     */
    void setCoefficient(dsp_float r);

    /**
     * @brief Processes a single input sample and returns the DC-blocked output.
     * @param input The current input sample x[n].
     * @return Filtered output sample y[n].
     */
    dsp_float processSample(dsp_float input);

private:
    dsp_float x1; ///< Previous input sample x[n-1]
    dsp_float y1; ///< Previous output sample y[n-1]
    dsp_float R;  ///< Feedback coefficient controlling low-cut frequency
};
