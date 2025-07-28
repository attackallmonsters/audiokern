#pragma once

#include "dsp_types.h"
#include "clamp.h"
#include <cmath>

/**
 * @brief Utility class for normalizing MIDI controller values to audio-friendly ranges.
 *
 * This class provides standard conversions from 7- or 14-bit MIDI values
 * (e.g. velocity, pitch bend, modulation) to a floating-point range of -1.0 to 1.0.
 * Optional loudness scaling (RMS-style) can be applied to velocity and modulation.
 *
 * Typical applications:
 * - Velocity-sensitive amplitude or filter control
 * - Scaled pitch bend for detuning or frequency shift
 * - Mapping MIDI modulation to DSP parameters
 *
 * @code
 * MidiProcessor midi;
 * midi.initialize();
 * midi.setLoudnessScale(0.5);  // Use softer scaling if needed
 *
 * host_float v = midi.normalizeVelocity(64);         // → ~0.5
 * host_float pb = midi.normalizePitchBend(8192);     // → 0.0 (center)
 * host_float mod = midi.normalizeModulation(127);    // → 1.0
 * @endcode
 *
 * @note This class performs purely mathematical transformations. It holds no MIDI state.
 */
class MidiProcessor
{
public:
    /**
     * @brief Initializes the processor and sets default parameters.
     */
    void initialize();

    /**
     * @brief Sets the loudness scaling exponent for RMS-like behavior.
     *
     * Applied to velocity and modulation when using RMS versions.
     * Typical values range from 0.3 to 0.7.
     *
     * @param scale Exponent used in pow(..., scale)
     */
    void setLoudnessScale(host_float scale);

    /**
     * @brief Normalizes velocity (0–127) to linear range -1.0 to 1.0.
     *
     * @param velocity MIDI velocity (0–127)
     * @return Normalized value (0.0 to ~1.0)
     */
    host_float normalizeVelocity(host_float velocity);

    /**
     * @brief Normalizes velocity (0–127) to RMS-scaled range -1.0 to 1.0.
     *
     * Applies loudness exponent to approximate perceived intensity.
     *
     * @param velocity MIDI velocity (0–127)
     * @return RMS-scaled value (0.0 to ~1.0)
     */
    host_float normalizeVelocityRMS(host_float velocity);

    /**
     * @brief Normalizes pitch bend (0–16383) to range -1.0 to 1.0.
     *
     * @param bend 14-bit MIDI pitch bend value (default center is 8192)
     * @return Normalized bipolar value
     */
    host_float normalizePitchBend(host_float bend);

    /**
     * @brief RMS-scaled version of normalizePitchBend().
     *
     * @param bend 14-bit MIDI pitch bend value
     * @return Normalized and scaled value
     */
    host_float normalizePitchBendRMS(host_float bend);

    /**
     * @brief Normalizes modulation value (0–127) to range -1.0 to 1.0.
     *
     * @param mod MIDI modulation value
     * @return Normalized modulation depth
     */
    host_float normalizeModulation(host_float mod);

    /**
     * @brief RMS-scaled version of normalizeModulation().
     *
     * @param mod MIDI modulation value
     * @return RMS-scaled modulation depth
     */
    host_float normalizeModulationRMS(host_float mod);

private:
    host_float loudnessScale = 1.0; ///< Exponent used for RMS scaling
};
