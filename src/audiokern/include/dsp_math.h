#pragma once

#include "dsp_types.h"
#include "clamp.h"
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

    // Internal LUTs using function-local statics for C++11 compatibility
    inline std::array<host_float, LUT_SIZE + 1>& getSinLUT() {
        static std::array<host_float, LUT_SIZE + 1> sinLUT{};
        return sinLUT;
    }

    inline std::array<host_float, LUT_SIZE + 1>& getCosLUT() {
        static std::array<host_float, LUT_SIZE + 1> cosLUT{};
        return cosLUT;
    }

    inline bool& isLutInitialized() {
        static bool lut_initialized = false;
        return lut_initialized;
    }

    /**
     * @brief Maps a normalized float value [0.0, 1.0] to an integer range [min, max].
     */
    inline int normf_to_int_range(float norm, int min, int max)
    {
        norm = clamp(norm, 0.0f, 1.0f);
        return static_cast<int>(std::round(min + norm * (max - min)));
    }

    /**
     * @brief Fast tanh approximation for waveshaping or saturation.
     */
    inline host_float fast_tanh(host_float val)
    {
        if (val < -3.0) return -1.0;
        if (val > 3.0) return 1.0;
        host_float val2 = val * val;
        return val * (27.0 + val2) / (27.0 + 9.0 * val2);
    }

    /**
     * @brief Soft clipping function for smooth distortion.
     */
    inline host_float soft_clip(host_float x)
    {
        constexpr host_float threshold = 2.5;
        if (x < -threshold) return -1.0;
        if (x > threshold) return 1.0;
        return x * (1.0 - (x * x) / (3.0 * threshold * threshold));
    }

    /**
     * @brief Enum defining time ratios.
     */
    enum class TimeRatio
    {
        NONE, HALF, DOUBLE, TRIPLET, DOTTED,
        POLY_3_4, POLY_4_3, POLY_3_5, POLY_5_3,
        GOLDEN_RATIO, SILVER_RATIO, PLATINUM_RATIO,
        SQRT_2, SQRT_3, PHI_INV, PI_REL, E_REL
    };

    /**
     * @brief Applies a time ratio to a given base time value.
     */
    inline host_float getTimeRatio(host_float time, TimeRatio ratio)
    {
        switch (ratio)
        {
        case TimeRatio::NONE: return time;
        case TimeRatio::HALF: return time * 0.5;
        case TimeRatio::DOUBLE: return time * 2.0;
        case TimeRatio::TRIPLET: return time * (2.0 / 3.0);
        case TimeRatio::DOTTED: return time * (3.0 / 2.0);
        case TimeRatio::POLY_3_4: return time * (3.0 / 4.0);
        case TimeRatio::POLY_4_3: return time * (4.0 / 3.0);
        case TimeRatio::POLY_3_5: return time * (5.0 / 3.0);
        case TimeRatio::POLY_5_3: return time * (3.0 / 5.0);
        case TimeRatio::GOLDEN_RATIO: return time * 0.6180339887;
        case TimeRatio::SILVER_RATIO: return time * 0.4142135623;
        case TimeRatio::PLATINUM_RATIO: return time * 0.3333333333;
        case TimeRatio::SQRT_2: return time * std::sqrt(2.0);
        case TimeRatio::SQRT_3: return time * std::sqrt(3.0);
        case TimeRatio::PHI_INV: return time * 1.6180339887;
        case TimeRatio::PI_REL: return time * DSP_PI;
        case TimeRatio::E_REL: return time * std::exp(1.0);
        default: return time;
        }
    }

    /**
     * @brief Initializes sine and cosine lookup tables.
     */
    inline void init_trig_lut()
    {
        if (isLutInitialized()) return;
        
        auto& sinLUT = getSinLUT();
        auto& cosLUT = getCosLUT();
        
        for (int i = 0; i <= LUT_SIZE; ++i)
        {
            host_float omega = i * LUT_RESOLUTION;
            sinLUT[i] = std::sin(omega);
            cosLUT[i] = std::cos(omega);
        }
        isLutInitialized() = true;
    }

    /**
     * @brief Computes equal-power panning coefficients from angle.
     */
    inline void get_sin_cos(host_float omega, host_float *cos_out, host_float *sin_out)
    {
        // Auto-initialize if not done yet
        if (!isLutInitialized()) {
            init_trig_lut();
        }
        
        omega = clamp(omega, 0.0, DSP_PI);
        int index = static_cast<int>(omega / LUT_RESOLUTION + 0.5);
        if (index > LUT_SIZE) index = LUT_SIZE;
        
        auto& sinLUT = getSinLUT();
        auto& cosLUT = getCosLUT();
        
        *sin_out = sinLUT[index];
        *cos_out = cosLUT[index];
    }

    /**
     * @brief Generates a unique string identifier using timestamp and counter.
     */
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