#include "AnalogDrift.h"
#include "clamp.h"
#include <cstdlib>

AnalogDrift::AnalogDrift()
{
}

AnalogDrift::~AnalogDrift()
{
}

/**
 * @brief Initializes the drift system.
 *
 * Resets internal drift to zero. Called once during DSP initialization.
 */
void AnalogDrift::initializeObject()
{
    drift = 0.0f;
    setAmount(1.0);
    setDamping(0.999);
}

/**
 * @brief Sets the maximum drift amount per update step.
 */
void AnalogDrift::setAmount(host_float a)
{
    amount = clampmin(a, 0.0);
}

/**
 * @brief Sets the damping factor (smoothing).
 */
void AnalogDrift::setDamping(host_float d)
{
    damping = 1 - clamp(d, 0.0, 0.999);
}

/**
 * @brief Returns the current drift value.
 */
host_float AnalogDrift::getDrift()
{
    // TODO
    
    
    // Generate a random value in range [-1, 1]
    host_float randomStep = ((std::rand() / (host_float)RAND_MAX) * 2.0f) - 1.0f;

    // Scale by step amount
    randomStep *= amount;

    // Apply damping to simulate inertia
    drift = (drift * damping) + randomStep;

    return drift;
}