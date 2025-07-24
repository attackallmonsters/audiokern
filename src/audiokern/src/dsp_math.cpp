#include "dsp_math.h"
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
