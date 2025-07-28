#pragma once

#include "dsp_types.h"
#include <cmath>
#include <array>
#include <chrono>
#include <sstream>
#include <atomic>

/**
 * @brief Namespace for common DSP math utilities and constants.
 *
 * Contains lookup tables, fast approximations, and helper functions
 * for trigonometry and signal shaping.
 */
namespace dsp_math
{
    /** @brief Size of internal sine/cosine lookup tables */
    constexpr int LUT_SIZE = 4096;

    /** @brief Value of π (pi) with double precision */
    constexpr host_float DSP_PI = 3.14159265358979323846;

    /** @brief Value of 2π (tau) */
    constexpr host_float DSP_2PI = 6.28318530717958647692;

    /** @brief 1 divided by square root of 2 (used for Q=0.707 filters) */
    constexpr host_float DSP_1D_SQRT2 = 0.70710678118;

    /** @brief Step size in radians for LUT indexing (π / LUT_SIZE) */
    constexpr host_float LUT_RESOLUTION = DSP_PI / LUT_SIZE;

    /**
     * @brief Maps a normalized float value [0.0, 1.0] to an integer range [min, max].
     * 
     * Values outside [0.0, 1.0] are clamped before mapping.
     * 
     * @param norm Normalized float input (expected in range [0.0, 1.0]).
     * @param min Minimum integer output value.
     * @param max Maximum integer output value.
     * @return int Resulting integer in range [min, max].
     */
    int normf_to_int_range(float norm, int min, int max);

    /**
     * @brief Fast tanh approximation for waveshaping or saturation.
     *
     * Usually implemented as a polynomial or clipped rational approximation.
     *
     * @param val Input value.
     * @return Approximated tanh(val).
     */
    host_float fast_tanh(host_float val);

    /**
     * @brief Soft clipping function for smooth distortion.
     *
     * Clamps the signal near ±1.0 with a nonlinear transition.
     *
     * @param x Input signal.
     * @return Soft-clipped signal.
     */
    host_float soft_clip(host_float x);

    /**
     * @brief Enum defining commonly used or mathematically interesting time ratios.
     *
     * These values represent multipliers that can be applied to a base time value
     * to derive a related secondary time value. Useful in contexts like timing,
     * modulation, rhythmic relationships, or spatial effects.
     */
    enum class TimeRatio
    {
        NONE,   ///< No change; the secondary time remains equal to the base time.
        HALF,   ///< Multiplies base time by 0.5 (half-time).
        DOUBLE, ///< Multiplies base time by 2.0 (double-time).

        TRIPLET, ///< Two-thirds of the base time (common in triplet rhythms).
        DOTTED,  ///< One and a half times the base time (dotted rhythm).

        POLY_3_4, ///< Ratio 3:4 – often used for subtle time offsets.
        POLY_4_3, ///< Ratio 4:3 – slight temporal expansion.
        POLY_3_5, ///< Ratio 3:5 – irregular relationship for added complexity.
        POLY_5_3, ///< Ratio 5:3 – inverse of 3:5, for tighter relation.

        GOLDEN_RATIO,   ///< Approximately 0.618 – the inverse of the golden ratio φ.
        SILVER_RATIO,   ///< Approximately 0.414 – 1 / (1 + √2), less common but useful.
        PLATINUM_RATIO, ///< Exactly 1/3 – simple subdivision.

        SQRT_2, ///< Square root of 2 (~1.414) – geometric midpoint between 1× and 2×.
        SQRT_3, ///< Square root of 3 (~1.732) – broader irrational relationship.

        PHI_INV, ///< The golden ratio φ (~1.618) – inverse of GOLDEN_RATIO.
        PI_REL,  ///< Pi (~3.14159) – useful for irrational, circular or non-repeating timings.
        E_REL    ///< Euler’s number e (~2.71828) – exponential time scaling or growth.
    };

    /**
     * @brief Applies a time ratio to a given base time value.
     *
     * This function returns a scaled time value based on the specified ratio.
     * It can be used in contexts such as rhythmic relationships, modulation timing,
     * signal delays, or other time-based processing scenarios.
     *
     * @param time  The base time value to scale.
     * @param ratio The ratio to apply to the base time.
     * @return host_float The resulting time value after applying the ratio.
     */
    host_float getTimeRatio(host_float time, TimeRatio ratio);

    /**
     * @brief Precomputed sine lookup table from 0 to π.
     *
     * Index range: [0, LUT_SIZE]
     */
    extern std::array<host_float, LUT_SIZE + 1> sinLUT;

    /**
     * @brief Precomputed cosine lookup table from 0 to π.
     *
     * Index range: [0, LUT_SIZE]
     */
    extern std::array<host_float, LUT_SIZE + 1> cosLUT;

    /**
     * @brief Indicates whether the lookup tables have been initialized.
     */
    extern bool lut_initialized;

    /**
     * @brief Initializes sine and cosine lookup tables.
     *
     * Must be called once before using `get_sin_cos`.
     */
    void init_trig_lut();

    /**
     * @brief Computes equal-power panning coefficients from angle.
     *
     * Uses LUTs if initialized. Converts phase angle (omega) into cosine and sine values:
     * - `cos_out` = left gain
     * - `sin_out` = right gain
     *
     * @param omega Angle in radians (typically 0 to π/2).
     * @param cos_out Pointer to receive cos(omega).
     * @param sin_out Pointer to receive sin(omega).
     */
    void get_sin_cos(host_float omega, host_float *cos_out, host_float *sin_out);

    /**
     * @brief Generates a unique string identifier using timestamp and counter.
     *
     * Useful for dynamic naming of temporary buffers, instances, etc.
     *
     * @param name Base string to prefix.
     * @return Unique string (e.g., "mod1234567").
     */
    std::string unique_string_id(const std::string &name);
}
