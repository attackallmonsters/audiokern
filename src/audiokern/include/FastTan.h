#pragma once

#include "SoundProcessor.h"
#include "DSPSampleBuffer.h"
#include "dsp_types.h"
#include "dsp_math.h"

/**
 * @brief A signal processor that applies a fast approximation of the tangent function.
 * 
 * This processor transforms the audio signal using a non-linear function (e.g. fast tanh or tan approximation)
 * for experimental shaping, distortion, or analog-style behavior. It processes both left and right audio buffers.
 * 
 * Intended use: creative waveshaping, experimental synthesis, analog modeling.
 */
class FastTan : public SoundProcessor
{
public:
    /**
     * @brief Constructs a new FastTan processor instance.
     * 
     * Registers the internal processBlock function for DSP execution.
     */
    FastTan();

private:
    /**
     * @brief Static processing entry point for the DSP system.
     * 
     * This method is called by the DSP runtime and delegates the call
     * to the instance-level processBlock() method.
     * 
     * @param dsp Pointer to the DSPObject (cast to FastTan internally)
     */
    static void processBlock(DSPObject *dsp);

    /**
     * @brief Internal processing method.
     * 
     * Applies a fast tangent approximation to each sample in the input buffers,
     * writing the result back into the same buffers.
     */
    void processBlock();
};
