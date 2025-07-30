#pragma once
#include "dsp_types.h"
#include <cstdint>
#include <chrono>

/**
 * @brief Minimalist real-time random number generator.
 *        Auto-seeded on first use from high-resolution clock.
 *        Suitable for real-time DSP applications.
 */
namespace dsp_rnd
{
    static uint32_t state = 0;
    static bool seeded = false;

    /**
     * @brief Initializes RNG using high-resolution clock as seed.
     */
    inline void seed()
    {
        using namespace std::chrono;
        auto now = high_resolution_clock::now().time_since_epoch();
        state = static_cast<uint32_t>(now.count() & 0xFFFFFFFF);
        seeded = true;
    }

    inline void initialize()
    {
        seed();
    }

    /**
     * @brief Returns next raw pseudo-random value.
     */
    inline uint32_t next_raw()
    {
        state = 1664525u * state + 1013904223u;
        return state;
    }

    /**
     * @brief Returns int in [min, max] inclusive.
     */
    inline int next(int min, int max)
    {
        uint32_t r = next_raw();
        return min + static_cast<int>(r % (max - min + 1));
    }

    /**
     * @brief Returns float in [0.0, 1.0)
     */
    inline host_float next_float()
    {
        return (next_raw() & 0xFFFFFF) / 16777216.0f;
    }

    /**
     * @brief Returns float in [min, max)
     */
    inline host_float next_float(host_float min, host_float max)
    {
        return min + (max - min) * next_float();
    }
}
