#pragma once

#include "SoundEffect.h"
#include "dsp_types.h"

/**
 * @brief A distortion effect that adds harmonic saturation and clipping to audio signals
 *
 * The Distortion class provides various types of distortion including soft clipping,
 * hard clipping, and saturation. It processes stereo audio through configurable
 * distortion algorithms with adjustable drive, output gain, and tone shaping.
 *
 * Usage:
 * - Call setDrive() to control the amount of distortion applied
 * - Call setOutputGain() to adjust the output level after distortion
 * - Call setDistortionType() to select between different distortion algorithms
 * - Call setTone() to adjust the frequency response characteristics
 * 
 * The distortion effect supports modulation of drive amount through the modulation bus.
 * Input and output buses handle stereo audio processing with parallel left/right channels.
 * 
 * Background:
 * This distortion implementation uses various mathematical functions to create
 * harmonic content and signal saturation. Soft clipping uses hyperbolic tangent
 * for smooth saturation, while hard clipping provides more aggressive limiting.
 * Tube-style saturation simulates vacuum tube characteristics.
 * 
 * Example:
 * @code
 * Distortion distortion;
 * distortion.connectInputToBus(inputBus);
 * distortion.connectOutputToBus(outputBus);
 * distortion.setDrive(0.7f);
 * distortion.setDistortionType(DistortionType::SoftClip);
 * @endcode
 */
class Distortion : public SoundEffect
{
public:
    /**
     * @brief Available distortion algorithms
     */
    enum class DistortionType
    {
        SoftClip,    ///< Smooth hyperbolic tangent saturation
        HardClip,    ///< Sharp amplitude limiting
        Tube,        ///< Vacuum tube-style asymmetric saturation
        Foldback     ///< Wave folding distortion
    };

    Distortion();

    /**
     * @brief Sets the drive amount controlling distortion intensity
     * 
     * Controls how much the input signal is amplified before distortion
     * processing. Higher values create more aggressive distortion effects.
     *
     * @param drive Drive amount (0.0 to 1.0, where 0.0 is clean and 1.0 is maximum distortion)
     */
    void setDrive(host_float drive);

    /**
     * @brief Sets the output gain applied after distortion processing
     * 
     * Compensates for level changes introduced by the distortion algorithm
     * and allows final output level adjustment.
     *
     * @param gain Output gain multiplier (0.0 to 2.0, where 1.0 is unity gain)
     */
    void setOutputGain(host_float gain);

    /**
     * @brief Selects the distortion algorithm to use
     * 
     * Different algorithms provide varying harmonic characteristics and
     * response curves for different musical applications.
     *
     * @param type The distortion algorithm to apply
     */
    void setDistortionType(DistortionType type);

    /**
     * @brief Sets the tone control affecting frequency response
     * 
     * Adjusts the high-frequency content before distortion processing
     * to shape the harmonic character of the distorted signal.
     *
     * @param tone Tone control (-1.0 to 1.0, where -1.0 is dark, 0.0 is neutral, 1.0 is bright)
     */
    void setTone(host_float tone);

protected:
    /**
     * @brief Initializes the distortion effect with default parameters
     */
    void initializeEffect() override;

    /**
     * @brief Initializes the distortion effect for polyphonic operation
     * 
     * @param count Number of voices for polyphonic processing
     */
    void initializeEffect(size_t count) override;

private:
    /// @brief Static dispatcher for soft clip processing
    static void processSoftClip(DSPObject *dsp);

    /// @brief Static dispatcher for hard clip processing
    static void processHardClip(DSPObject *dsp);

    /// @brief Static dispatcher for tube saturation processing
    static void processTube(DSPObject *dsp);

    /// @brief Static dispatcher for foldback processing
    static void processFoldback(DSPObject *dsp);

    /// @brief Internal soft clip block processing
    void processSoftClip();

    /// @brief Internal hard clip block processing
    void processHardClip();

    /// @brief Internal tube saturation block processing
    void processTube();

    /// @brief Internal foldback block processing
    void processFoldback();

    /// @brief Registers the appropriate block processor based on distortion type
    void updateBlockProcessor();

    /// @brief Drive amount (0.0 to 1.0)
    host_float drive;

    /// @brief Output gain multiplier
    host_float outputGain;

    /// @brief Current distortion algorithm
    DistortionType distortionType;

    /// @brief Tone control value (-1.0 to 1.0)
    host_float tone;

    /// @brief Pre-emphasis filter coefficients for tone shaping
    host_float toneFilterCoeff;

    /// @brief Filter state for left channel tone control
    host_float toneFilterStateL;

    /// @brief Filter state for right channel tone control
    host_float toneFilterStateR;
};