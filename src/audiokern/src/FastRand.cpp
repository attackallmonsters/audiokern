#include "FastRand.h"
#include <chrono>   // Only used in constructor
#include <cstdint>

FastRand::FastRand()
{
    seed();
}

unsigned int FastRand::next()
{
    // Linear Congruential Generator: X_{n+1} = a * X_n + c (mod 2^32)
    current = current * 1664525u + 1013904223u;
    return current;
}

float FastRand::nextRandomSample()
{
    return static_cast<float>(next()) / 4294967296.0f; // 2^32
}

void FastRand::reseed(unsigned int seed)
{
    current = seed;
}

unsigned int FastRand::seed()
{
    // Combine high.mes clock and stack address into a seed
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    const void* addr = static_cast<const void*>(&now); // Stack address as entropy

    return static_cast<unsigned int>(
        now ^ reinterpret_cast<uintptr_t>(addr)
    );
}
