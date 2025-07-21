#include "dsp_math.h"
#include <cmath>

namespace dsp_math
{
    std::array<dsp_float, LUT_SIZE + 1> sinLUT;
    std::array<dsp_float, LUT_SIZE + 1> cosLUT;
    bool lut_initialized = false;

    void init_trig_lut()
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

    void get_sin_cos(dsp_float omega, dsp_float *sin_out, dsp_float *cos_out)
    {
        omega = std::fmin(std::fmax(0.0, omega), dsp_math::DSP_PI);
        int index = static_cast<int>(omega / LUT_RESOLUTION + 0.5);
        if (index > LUT_SIZE)
            index = LUT_SIZE;

        *sin_out = sinLUT[index];
        *cos_out = cosLUT[index];
    }
}
