#pragma once

#include <functional>
#include <queue>
#include "DSP.h"
#include "DSPSampleBuffer.h"
#include "SoundProcessor.h"
#include "dsp_types.h"

/**
 * @brief Utility DSP processor that fades audio before and after applying parameter changes.
 *
 * `ParamFader` is a helper block used to apply potentially disruptive parameter changes
 * (such as oscillator detune, frequency shifts, FM index updates, etc.) in a smooth and
 * click-free manner. It does this by fading the audio output down to zero,
 * applying the parameter change, and fading back up — all within a configurable sample duration.
 *
 * Changes are queued via `change(...)` and applied one at a time per audio block.
 *
 * ### Usage Example
 * @code
 * ParamFader fader;
 * fader.initialize();
 *
 * // Schedule a detune change for smooth application
 * fader.change([=]() {
 *     osc->setDetune(newDetune);
 * });
 *
 * DSP::connect(osc, fader);
 * DSP::connect(fader, output);
 * fader.process(); // fades out, applies change, fades in
 * @endcode
 *
 * @note The parameter change itself is executed sample-synchronously, but the fade occurs over
 *       `fadeLength` samples (default is 16). This prevents audible pops or clicks.
 * @note The fade affects the audio signal only — parameters are changed instantaneously after fade-out.
 */
class ParamFader : public SoundProcessor
{
public:
    /**
     * @brief Constructs a new ParamFader instance.
     *
     * Initializes fade parameters and clears any change queue.
     */
    ParamFader();

    /**
     * @brief Defines a deferred parameter change function.
     *
     * This type is typically a lambda or callable that performs a DSP parameter update.
     */
    using ParamChange = std::function<void()>;

    /**
     * @brief Queues a parameter change to be applied safely.
     *
     * The change will be applied after the output signal fades out to silence,
     * and then the output will fade back in over `fadeLength` samples.
     *
     * @param fn The function that modifies DSP parameters.
     */
    void change(ParamChange fn);

private:
    /**
     * @brief Static entry point for DSP block processing.
     * @param dsp Pointer to the ParamFader instance (cast from DSPObject).
     */
    static void processBlock(DSPObject *dsp);

    /**
     * @brief Internal implementation of audio fade and parameter application.
     */
    void processBlock();

    std::queue<ParamChange> changes;  ///< Queue of scheduled parameter change functions
    int fadeCounter = 0;              ///< Tracks current fade progress
    const int fadeLength = 16;        ///< Number of samples for fade-out and fade-in
    dsp_float fadeValue = 1.0;        ///< Current gain multiplier during fade
    bool applyParamChange = false;    ///< Indicates that a change should be applied mid-fade
};
