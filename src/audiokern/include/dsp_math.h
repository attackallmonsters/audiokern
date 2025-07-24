#pragma once

#include "dsp_types.h"
#include <cmath>
#include <array>
#include <chrono>
#include <sstream>
#include <atomic>

namespace dsp_math
{
    constexpr int LUT_SIZE = 2048;
    constexpr host_float DSP_PI = 3.14159265358979323846;
    constexpr host_float DSP_2PI = 6.28318530717958647692;
    constexpr host_float DSP_1D_SQRT2 = 0.70710678118; // 1/sqrt(2)
    constexpr host_float LUT_RESOLUTION = DSP_PI / LUT_SIZE;

    // Fast tanh approximation
    host_float fast_tanh(host_float val);

    // Soft clipper
    host_float soft_clip(host_float x);

    // Lookup table declarations (müssen extern sein!)
    extern std::array<host_float, LUT_SIZE + 1> sinLUT;
    extern std::array<host_float, LUT_SIZE + 1> cosLUT;
    extern bool lut_initialized;

    void init_trig_lut();

    /**
     * @brief Equal power caluclation: cos_out = left, sin_out = right
     */
    void get_sin_cos(host_float omega, host_float *cos_out, host_float *sin_out);

    /**
     * @brief Creates a unique string id
     */
    std::string unique_string_id(const std::string &name);
}
