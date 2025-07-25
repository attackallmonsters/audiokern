#pragma once

#include "SoundEffect.h"
#include "CombDelay.h"
#include "clamp.h"
#include "CrossFader.h"
#include <array>
#include <vector>

/**
 * @brief NebularReverb implements a metallic, plate-style reverb using parallel comb filters.
 *
 * This stereo reverb effect consists of multiple short feedback-based delay lines,
 * each acting as a comb filter. Its character is metallic and dense, making it suitable
 * for synthetic or experimental use. Space and room size affect time distribution and
 * feedback amount, while damping controls a lowpass filter in the feedback path.
 *
 * - Best results are achieved with high feedback (90–100%) and short delays (0–10% space).
 * - All parameters are real-time controllable.
 */
class NebularReverb : public SoundEffect
{
public:
    /**
     * @brief Default constructor.
     */
    NebularReverb();

    /**
     * @brief Sets the number of parallel comb delays.
     * @param dense Number of lines (clamped between 2 and 8).
     */
    void setDensity(host_float dense);

    /**
     * @brief Controls spread between delay lines.
     * @param size Value in [0.0, 1.0], higher means more time offset.
     */
    void setSpace(host_float size);

    /**
     * @brief Sets damping filter frequency.
     * @param d Frequency in Hz (0 – 20000), used as lowpass cutoff in feedback.
     */
    void setDamping(host_float d);

    /**
     * @brief Controls feedback gain (tail length).
     * @param size Value in [0.0, 1.0]. Higher values increase decay time.
     *             Note: 1.0 may result in self-oscillation.
     */
    void setRoomSize(host_float size);

    /**
     * @brief Sets the output wet signal level.
     * @param vol Volume factor [0.0, 1.0].
     */
    void setWet(host_float vol);

    /**
     * @brief Sets the time ratio to be applied within the reverb processing structure.
     *
     * This ratio is used to scale internal delay times. Applying different ratios can help to avoid
     * resonances, increase stereo width, and introduce natural-sounding temporal variation.
     *
     * @param ratio The time ratio to apply to relevant time-based components within the reverb.
     */
    void setTimeRatio(dsp_math::TimeRatio ratio);

protected:
    /**
     * @brief Called during DSP activation. Prepares all delay lines and faders.
     */
    void initializeEffect() override;

    /**
     * @brief Called when the input bus is connected. Not used in current implementation.
     */
    void onInputBusConnected();

    /**
     * @brief Called when the output bus is connected. Prepares fader connection.
     */
    void onOutputBusConnected();

private:
    /**
     * @brief Static DSP processing callback.
     * Dispatches to the instance-level `processBlock()`.
     */
    static void processBlock(DSPObject *dsp);

    /**
     * @brief Processes one audio block.
     * Mixes the outputs of all active delay lines and applies dry/wet crossfade.
     */
    void processBlock();

    /**
     * @brief Updates internal delay lines based on the current parameters.
     * Called after each parameter change.
     */
    void updateDelays();

    /// Maximum number of comb delay lines allowed
    static constexpr int maxDelays = 12;

    /// Number of active delay lines, controlled by density
    int density;

    /// Base delay time (spread between lines)
    host_float delayTime;

    /// Output wet level
    host_float wet;

    /// CombDelay time relations L/R
    dsp_math::TimeRatio timeRatio;

    /// All comb delay instances
    std::vector<CombDelay *> delays;

    /// Unique names for bus registration
    std::vector<std::string> delayNames;

    /// Pointers to output buses of delay lines
    std::vector<DSPAudioBus *> delayBusses;

    /// Wet buffer
    DSPAudioBus *wetBus;

    /// Crossfader for dry/wet mixing
    CrossFader wetFader;
};
