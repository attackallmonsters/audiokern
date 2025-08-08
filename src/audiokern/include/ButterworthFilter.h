#pragma once

#include "SoundProcessor.h"
#include "DSPSampleBuffer.h"
#include "VoiceOptions.h"
#include "dsp_types.h"
#include "dsp_math.h"
#include "clamp.h"
#include "SlewLimiter.h"
#include <cmath>

/**
 * @brief Simple second-order Butterworth filter (lowpass or highpass).
 *
 * This filter supports stereo processing and provides runtime configuration
 * of the cutoff frequency and filter mode. It does not support modulation—
 * the cutoff frequency must be set statically via setCutoffFrequency().
 */
class ButterworthFilter : public SoundProcessor
{
public:
    /**
     * @brief Constructs a ButterworthFilter with default parameters.
     *
     * Initializes internal state and sets the default filter mode to lowpass.
     * The filter must be initialized and connected before processing.
     */
    explicit ButterworthFilter();

    /**
     * @brief Sets the cutoff frequency of the filter in Hz.
     *
     * Must be called before processing. Changing the frequency per block
     * is allowed, but modulation via external modulation buses is not supported.
     *
     * @param freq Cutoff frequency in Hz (e.g., 2000.0)
     */
    void setCutoffFrequency(host_float freq);

    /**
     * @brief Resets the internal filter state.
     *
     * Clears past input/output samples to avoid clicks or instability when reinitializing.
     */
    void reset();

    /**
     * @brief Sets the filter mode.
     *
     * Supports lowpass and highpass filtering using a second-order biquad structure.
     *
     * @param mode Filter mode (FilterMode::LP or FilterMode::HP)
     */
    void setFilterMode(FilterMode mode);

protected:
    /**
     * @brief Called once when the DSP object is registered and initialized.
     *
     * Connects to the standard output bus and prepares internal buffers.
     * No modulation bus will be connected.
     */
    void initializeProcessor() override;

private:
    /// Cutoff frequency in Hz (must be positive and < Nyquist)
    host_float cutoffFrequency;

    /// Internal biquad filter state for left channel
    host_float z1L = 0.0, z2L = 0.0;

    /// Internal biquad filter state for right channel
    host_float z1R = 0.0, z2R = 0.0;

    /// Current filter mode (lowpass or highpass)
    FilterMode filterMode = FilterMode::LP;

    /// Static wrapper for processBlock() used in function registration
    static void processBlock(DSPObject *dsp);

    /// Processes one block of audio samples.
    void processBlock();

    /// @brief Used for changing cutoff to avoid clicking
    SlewLimiter slew;

    /// Past input/output samples for left channel (used in biquad calculation)
    host_float x1L, y1L, x2L, y2L;

    /// Past input/output samples for right channel (used in biquad calculation)
    host_float x1R, y1R, x2R, y2R;
};
