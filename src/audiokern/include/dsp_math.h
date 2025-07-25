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
