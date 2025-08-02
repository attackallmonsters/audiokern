#pragma once

#include "DSPObject.h"
#include "DSPBusManager.h"
#include "SlewLimiter.h"
#include "dsp_math.h"
#include "clamp.h"
#include <cmath>

#pragma once

#include "DSPObject.h"
#include "SlewLimiter.h"

/**
 * @brief A two-channel crossfader for audio signal blending.
 * 
 * This class blends two input audio buses (A and B) into a single output,
 * based on a continuous mix value between 0.0 (only A) and 1.0 (only B).
 * Slew limiting is applied to prevent audio artifacts (e.g. zipper noise).
 * 
 * Usage:
 * - Connect input buses via connectInputBusForA/B.
 * - Adjust mix via setMix().
 * - Output is written to this object's outputBus.
 */
class CrossFader : public DSPObject
{
public:
    /**
     * @brief Constructor.
     * 
     * Sets up internal state and registers the DSP callback.
     * Note: Actual bus connections must be done explicitly.
     */
    CrossFader();

    /**
     * @brief Connects the input audio bus A (typically the "dry" or original signal).
     * 
     * @param busName The name of the audio bus to connect to input A.
     */
    void connectInputAToBus(DSPAudioBus &bus);

    /**
     * @brief Connects the input audio bus B (typically the "wet" or processed signal).
     * 
     * @param busName The name of the audio bus to connect to input B.
     */
    void connectInputBToBus(DSPAudioBus &bus);

    /**
     * @brief Connects the output audio bus for the mix of A and B.
     * 
     * @param busName The name of the output audio bus.
     */
    void connectOutputToBus(DSPAudioBus &bus);

    /**
     * @brief Sets the mix balance between input A and input B.
     * 
     * @param value A floating-point value from 0.0 (only A) to 1.0 (only B).
     * Intermediate values blend linearly.
     */
    void setMix(double value);

protected:
    /**
     * @brief Initializes the instance and prepares internal buffers.
     * 
     * This method is called automatically by the DSP system once the block size is known.
     * It must not be called manually.
     */
    void initializeObject() override;

private:
    /**
     * @brief Static block processing entry point (callback).
     * 
     * Dispatches processing to the instance-level method.
     */
    static void processBlock(DSPObject *dsp);

    /**
     * @brief Processes the current audio block.
     * 
     * Applies gain blending between input A and B, based on the current mix value.
     * Slew limiting is used to ensure smooth transitions.
     */
    void processBlock(); 

    /// Slew limiter to avoid zipper noise when changing mix
    SlewLimiter slew;

    /// User-defined mix value (0.0 = full A, 1.0 = full B)
    double mix;

    /// Input bus A
    DSPAudioBus inputBusA;

    /// Input bus B
    DSPAudioBus inputBusB;

    /// Output bus
    DSPAudioBus outputBus;
};
