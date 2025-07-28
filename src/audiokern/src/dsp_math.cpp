#include "dsp_math.h"
#include "clamp.h"
#include <cmath>

namespace dsp_math
{
    host_float fast_tanh(host_float val)
    {
        if (val < -3.0)
            return -1.0;
        if (val > 3.0)
            return 1.0;

        const host_float val2 = val * val;
        return val * (27.0 + val2) / (27.0 + 9.0 * val2);
    }

    host_float soft_clip(host_float x)
    {
        const host_float threshold = 2.5;
        if (x < -threshold)
            return -1.0;
        else if (x > threshold)
            return 1.0;
        else
            return x * (1.0 - (x * x) / (3.0 * threshold * threshold));
    }

    int normf_to_int_range(float norm, int min, int max)
    {
        norm = clamp(norm, 0.0f, 1.0f);
        return static_cast<int>(std::round(min + norm * (max - min)));
    }

    host_float getTimeRatio(host_float time, TimeRatio ratio)
    {
        switch (ratio)
        {
        case TimeRatio::NONE:
            return time;
        case TimeRatio::HALF:
            return time * 0.5;
        case TimeRatio::DOUBLE:
            return time * 2.0;

        case TimeRatio::TRIPLET:
            return time * (2.0 / 3.0);
        case TimeRatio::DOTTED:
            return time * (3.0 / 2.0);

        case TimeRatio::POLY_3_4:
            return time * (3.0 / 4.0);
        case TimeRatio::POLY_4_3:
            return time * (4.0 / 3.0);
        case TimeRatio::POLY_3_5:
            return time * (5.0 / 3.0);
        case TimeRatio::POLY_5_3:
            return time * (3.0 / 5.0);

        case TimeRatio::GOLDEN_RATIO:
            return time * 0.6180339887; // 1 / φ
        case TimeRatio::SILVER_RATIO:
            return time * 0.4142135623; // 1 / (1 + √2)
        case TimeRatio::PLATINUM_RATIO:
            return time * 0.3333333333; // 1/3

        case TimeRatio::SQRT_2:
            return time * std::sqrt(2.0);
        case TimeRatio::SQRT_3:
            return time * std::sqrt(3.0);

        case TimeRatio::PHI_INV:
            return time * 1.6180339887; // φ
        case TimeRatio::PI_REL:
            return time * M_PI;
        case TimeRatio::E_REL:
            return time * std::exp(1.0);

        default:
            return time;
        }
    }

    std::array<host_float, LUT_SIZE + 1> sinLUT;
    std::array<host_float, LUT_SIZE + 1> cosLUT;
    bool lut_initialized = false;

    void init_trig_lut()
    {
        if (lut_initialized)
            return;

        for (int i = 0; i <= LUT_SIZE; ++i)
        {
            host_float omega = i * LUT_RESOLUTION;
            sinLUT[i] = std::sin(omega);
            cosLUT[i] = std::cos(omega);
        }

        lut_initialized = true;
    }

    void get_sin_cos(host_float omega, host_float *cos_out, host_float *sin_out)
    {
        omega = std::fmin(std::fmax(0.0, omega), dsp_math::DSP_PI);
        int index = static_cast<int>(omega / LUT_RESOLUTION + 0.5);
        if (index > LUT_SIZE)
            index = LUT_SIZE;

        *sin_out = sinLUT[index];
        *cos_out = cosLUT[index];
    }

    inline std::string unique_string_id(const std::string &name)
    {
        static std::atomic<uint64_t> counter{0};
        uint64_t count = counter.fetch_add(1);

        auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        const void *addr = static_cast<const void *>(&count);

        std::ostringstream ss;
        ss << name << "_" << now << "_" << count << std::hex << reinterpret_cast<uintptr_t>(addr);

        return ss.str();
    }
}
