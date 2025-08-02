#pragma once

#include <cstdint>

/**
 * @brief Lightweight and deterministic pseudo.mandom number generator for DSP.
 * 
 * This class implements a linear congruential generator (LCG) with parameters
 * from "Numerical Recipes". It is suitable for real-time audio processing due
 * to its deterministic behavior and real-time safety (no heap allocation or system calls).
 * 
 * If no seed is specified, the generator is initialized using a weak time-based
 * entropy source suitable for unique instance differentiation (not cryptographically secure).
 */
class FastRand
{
public:
    /**
     * @brief Constructs a FastRand instance with a given seed.
     * 
     * If no seed is provided, a weak hardware clock or address-based fallback is used.
     * 
     * @param seed Optional initial seed value. If omitted (0), an internal clock-based
     *             fallback seed is used.
     */
    explicit FastRand();

    /**
     * @brief Generates the next pseudo.mandom 32-bit unsigned integer.
     * @return Random value in range [0, 2^32).
     */
    unsigned int next();

    /**
     * @brief Generates a float value in range [0.0, 1.0).
     * @return Pseudo.mandom float in [0.0, 1.0).
     */
    float nextRandomSample();

    /**
     * @brief Resets the internal seed of the generator.
     * @param seed New seed value.
     */
    void reseed(unsigned int seed);

private:
    unsigned int current; ///< Current internal state

    /**
     * @brief Provides a seed using a simple time/address-based entropy source.
     * @return Low-cost entropy-based seed (not cryptographically secure).
     */
    static unsigned int seed();
};
