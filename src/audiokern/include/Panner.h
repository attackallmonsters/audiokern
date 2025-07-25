#pragma once

#include "SoundProcessor.h"
#include "DSPObject.h"
#include "DSPSampleBuffer.h"
#include "SlewLimiter.h"
#include "dsp_math.h"
#include "clamp.h"
#include <cmath>

/**
 * @brief Enum to define the panning behavior for stereo signal processing.
 *
 * This enum defines different strategies for mapping a stereo or mono input signal
 * across the stereo output field using equal-power panning laws.
 */
enum class PanningMode
{
    /**
     * @brief Basic gain-based panning (no crossfeed).
     *
     * - Each channel (L/R) is scaled independently using sin/cos gain law.
     * - No signal is mixed between channels.
     * - Left stays left, right stays right — just louder or quieter.
     * - Suitable when stereo separation must be preserved.
     */
    Gain,

    /**
     * @brief Equal-power crossfade panning with channel blending (not phase-safe).
     *
     * - Both left and right input signals are mixed into both output channels.
     * - Crossfeed allows a smoother pan and spatial motion effect.
     * - Preserves stereo image but may affect phase correlation.
     * - Useful for synths, effects, or animation of stereo position.
     */
    Blend,

    /**
     * @brief Mono summed equal-power panning (phase-safe).
     *
     * - Left and right inputs are summed to a single mono signal.
     * - Result is panned with equal-power law into both output channels.
     * - Ensures identical phase between channels.
     * - Ideal for centered sources or controlled stereo placement.
     */
    BlendMono
};

/**
 * @brief Stereo panning processor with equal-power law and multiple panning modes.
 *
 * This DSP processor applies stereo panning based on a mix value in the range [0.0, 1.0].
 * It supports three panning strategies:
 * - Gain: classic left/right gain adjustment (L and R kept separate)
 * - Blend: equal-power crossfade between L and R with stereo image preservation
 * - BlendMono: input is summed to mono, then panned with sin/cos weights
 *
 * Internally, a `SlewLimiter` ensures smooth transitions when `setPanning()` is updated.
 * Suitable for live modulation or automation of pan position.
 *
 * Usage example:
 * @code
 * Panner panner;
 * panner.initialize();
 * panner.setMode(PanningMode::Blend); // Use crossfeed blend mode
 * panner.setPanning(0.75);            // Pan 75% right
 *
 * DSP::connect(source, panner);
 * DSP::connect(panner, output);
 * panner.process();                   // Apply pan per block
 * @endcode
 *
 * @note Use `setPanning(0.5)` to center the signal.
 * @note Ideal for use after oscillators, filters, or FX where stereo placement is needed.
 */
class Panner : public SoundProcessor
{
public:
    /**
     * @brief Constructs an uninitialized stereo panner.
     *
     * Default mode is `PanningMode::Gain`, pan is centered (0.5).
     */
    Panner();

    /**
     * @brief Sets the current pan position (0.0 = hard left, 1.0 = hard right).
     *
     * The pan value is automatically slewed to avoid audio artifacts.
     *
     * @param value Pan position in range [0.0, 1.0].
     */
    void setPanning(double value);

    /**
     * @brief Sets the panning mode (Gain, Blend, BlendMono).
     *
     * Can be changed at runtime without reinitialization.
     *
     * @param mode Desired panning mode.
     */
    void setMode(PanningMode mode);

protected:
    /**
     * @brief Initializes the panner and registers the block processor.
     *
     * Called automatically during DSP graph setup.
     */
    void initializeProcessor() override;

private:
    /**
     * @brief Static processor for basic gain-based stereo panning.
     */
    static void processBlockGain(DSPObject *dsp);

    /**
     * @brief Static processor for crossfade stereo blend panning.
     */
    static void processBlockBlend(DSPObject *dsp);

    /**
     * @brief Static processor for mono-panned (L+R) signal.
     */
    static void processBlockBlendMono(DSPObject *dsp);

    /**
     * @brief Internal gain panning implementation.
     */
    void processBlockGain();

    /**
     * @brief Internal blend panning implementation.
     */
    void processBlockBlend();

    /**
     * @brief Internal mono-summed panning implementation.
     */
    void processBlockBlendMono();

    SlewLimiter slew;            ///< Smooths pan transitions over time
    double pan = 0.5;            ///< Current pan value (0.0–1.0), centered by default

    DSPAudioBus *inputBus = nullptr;  ///< Pointer to stereo input bus (L/R)
    DSPAudioBus *outputBus = nullptr; ///< Pointer to stereo output bus (L/R)
};
