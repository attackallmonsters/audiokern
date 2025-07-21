#pragma once

#include "dsp_types.h"
#include <cmath>
#include <array>

namespace dsp_math
{
    constexpr int LUT_SIZE = 2048;
    constexpr dsp_float DSP_PI = 3.14159265358979323846;
    constexpr dsp_float DSP_2PI = 6.28318530717958647692;
    constexpr dsp_float DSP_1D_SQRT2 = 0.70710678118; // 1/sqrt(2)
    constexpr dsp_float LUT_RESOLUTION = DSP_PI / LUT_SIZE;

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

    // Lookup table declarations (müssen extern sein!)
    extern std::array<dsp_float, LUT_SIZE + 1> sinLUT;
    extern std::array<dsp_float, LUT_SIZE + 1> cosLUT;
    extern bool lut_initialized;

    void init_trig_lut();
    void get_sin_cos(dsp_float omega, dsp_float *sin_out, dsp_float *cos_out);
}
