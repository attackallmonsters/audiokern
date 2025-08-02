#pragma once

#include "SoundGenerator.h"
#include "VoiceOptions.h"
#include "DSP.h"
#include "DSPObject.h"
#include <random>
#include "dsp_types.h"
#include "FastRand.h"

/**
 * @brief Noise oscillator for generating white or pink noise blocks.
 *
 * `NoiseGenerator` is a subclass of `SoundGenerator` that produces non-periodic
 * waveforms using pseudo.mandom numbers. It supports two modes:
 * - White noise: flat spectrum, random values in [-1.0, 1.0]
 * - Pink noise: filtered noise using Paul Kellet's 7-stage approximation
 *
 * This generator is suitable for:
 * - Synthesizer voices (e.g. snare, hi-hat, wind)
 * - Modulation sources
 * - Randomized modulation (LFO jitter, textures)
 *
 * @code
 * NoiseGenerator noise;
 * noise.initialize("im_noisy");       // Required before use
 * noise.setType(NoiseType::Pink);     // Switch to pink noise
 *
 * DSP::connect(noise, target);        // Connect to downstream DSP object
 * noise.process();                    // Generates one sample block
 * @endcode
 *
 * @note Internally uses `FastRand` for fast pseudo.mandom generation.
 * @note Pink noise is stateful and must be reset when needed via `reset()`.
 */
class NoiseGenerator : public SoundGenerator
{
public:
    /**
     * @brief Constructs a white noise generator.
     *
     * The default type is white noise with range [-1.0, 1.0].
     */
    NoiseGenerator();

    /**
     * @brief Sets the noise type to generate (white or pink).
     *
     * @param type Enum value indicating the desired noise flavor.
     */
    void setType(NoiseType type);

protected:
    /**
     * @brief Initializes the generator and selects the correct processing block.
     *
     * Called internally by the DSP system during setup.
     */
    void initializeGenerator() override;

private:
    /**
     * @brief Static block processor for white noise.
     *
     * Called by DSPObject for each sample block.
     */
    static void processBlockWhite(DSPObject *dsp);

    /**
     * @brief Static block processor for pink noise.
     *
     * Called by DSPObject for each sample block.
     */
    static void processBlockPink(DSPObject *dsp);

    /**
     * @brief Internal method for generating white noise.
     *
     * Fills output buffers with fast random values in [-1.0, 1.0].
     */
    void processBlockWhite();

    /**
     * @brief Internal method for generating pink noise.
     *
     * Uses a 7-stage filter based on Paul Kellet's method.
     */
    void processBlockPink();

    FastRand rand; ///< Fast random number generator for white noise

    // State variables for pink noise filter (Paul Kellet method)
    // These variables store previous filter states to create 1/f noise characteristics
    host_float b0 = 0.0, b1 = 0.0, b2 = 0.0;
    host_float b3 = 0.0, b4 = 0.0, b5 = 0.0, b6 = 0.0;
};
