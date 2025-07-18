#pragma once

#include <cmath>
#include <array>

namespace dsp_math
{
    // Mathematical constants
    // constexpr dsp_float M_PI     = 3.14159265358979323846;
    constexpr dsp_float M_2PI = 6.28318530717958647692;
    constexpr dsp_float M_1D_SQRT2 = 0.70710678118; // 1/sqrt(2)

    // Fast tanh approximation
    inline dsp_float fast_tanh(dsp_float val)
    {
        if (val < -3.0)
            return -1.0;
        if (val > 3.0)
            return 1.0;

        const dsp_float val2 = val * val;
        return val * (27.0 + val2) / (27.0 + 9.0 * val2);
    }

    // Soft clipper
    inline dsp_float soft_clip(dsp_float x)
    {
        const dsp_float threshold = 2.5;
        if (x < -threshold)
            return -1.0;
        else if (x > threshold)
            return 1.0;
        else
            return x * (1.0 - (x * x) / (3.0 * threshold * threshold));
    }

    // ----------------------------------------
    // Sin/Cos Lookup Table
    // ----------------------------------------

    constexpr int LUT_SIZE = 2048;
    constexpr dsp_float LUT_RESOLUTION = M_PI / LUT_SIZE;

    inline std::array<dsp_float, LUT_SIZE + 1> sinLUT;
    inline std::array<dsp_float, LUT_SIZE + 1> cosLUT;
    inline bool lut_initialized = false;

    inline void init_trig_lut()
    {
        if (lut_initialized)
            return;

        for (int i = 0; i <= LUT_SIZE; ++i)
        {
            dsp_float omega = i * LUT_RESOLUTION;
            sinLUT[i] = std::sin(omega);
            cosLUT[i] = std::cos(omega);
        }

        lut_initialized = true;
    }

    inline void get_sin_cos(dsp_float omega, dsp_float *sin_out, dsp_float *cos_out)
    {
        omega = std::fmin(std::fmax(0.0, omega), M_PI);
        int index = static_cast<int>(omega / LUT_RESOLUTION + 0.5);
        if (index > LUT_SIZE)
            index = LUT_SIZE;

        *sin_out = sinLUT[index];
        *cos_out = cosLUT[index];
    }
}
