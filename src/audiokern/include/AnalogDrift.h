#pragma once

#include "DSPObject.h"
#include "dsp_types.h"
#include <cstdlib>

/**
 * @brief Simulates analog-style frequency drift.
 *
 * AnalogDrift generates a slowly changing value that imitates the subtle frequency instability
 * of analog oscillators. This includes both continuous drift and jitter, based on a random walk.
 *
 * Usage:
 * - Call `setAmount(f)` to define maximum drift (e.g. 0.05 = ±5 cents).
 * - Call `setDamping(f)` to define how strongly drift resists fast changes (0.99–0.999 typical).
 * - Per audio block, `AnalogDrift` updates its internal drift using a random walk.
 * - Use `getDrift()` to retrieve the current drift value, which you can add to frequency etc.
 *
 * Recommended AnalogDrift settings for different applications.
 * 
 * These values define typical ranges for analog-style pitch or filter instability.
 * Adjust `amount` and `damping` depending on the desired effect and sensitivity.
 * 
 * | Target                | amount       | damping      | Drift Character                  |
 * |-----------------------|--------------|--------------|----------------------------------|
 * | Oscillator Detune     | 0.01 – 0.05  | 0.99         | lively, slightly unstable        |
 * | Supersaw Stack Drift  | 0.02 – 0.08  | 0.98         | uncorrelated, animated movement  |
 * | Filter Cutoff Mod     | 0.001 – 0.01 | 0.995 – 0.999| subtle, slow breathing motion    |
 *
 * Example:
 *   AnalogDrift drift;
 *   drift.setAmount(0.05f);  // ±5 cent range
 *   drift.setDamping(0.995f);
 *   drift.initialize();
 *   ...
 *   host_float detune = drift.getDrift();
 */
class AnalogDrift : public DSPObject
{
public:
    AnalogDrift();
    ~AnalogDrift();

    /**
     * @brief Sets the maximum drift amount.
     * 
     * Defines how far the drift can move per step.
     * Typical range: 0.01 to 0.1 (i.e. ±1 to ±10 cents in frequency).
     *
     * @param a Drift amount (unitless multiplier)
     */
    void setAmount(host_float a);

    /**
     * @brief Sets the damping factor for the random walk.
     * 
     * Higher values produce slower, more stable drift.
     * Typical range: 0.98 – 0.999.
     *
     * @param d Damping coefficient (0–1)
     */
    void setDamping(host_float d);

    /**
     * @brief Retrieves the current drift value.
     * 
     * This value changes slowly over time and can be added to pitch or frequency parameters.
     *
     * @return host_float Drift value (typically centered around 0)
     */
    host_float getDrift();

protected:
    /**
     * @brief Initializes internal state (called during DSP setup).
     */
    void initializeObject() override;

private:
    host_float drift = 0.0f;     ///< Current drift value
    host_float amount = 0.01f;   ///< Maximum drift step size
    host_float damping = 0.995f; ///< Drift persistence
};
