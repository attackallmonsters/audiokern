#pragma once

#include "SoundProcessor.h"
#include "dsp_types.h"
#include <cmath>

/**
 * @brief Simple PolyBLEP (Polynomial Band-Limited Step) sawtooth oscillator
 *
 * This class implements a basic PolyBLEP oscillator that generates band-limited 
 * sawtooth waves by applying polynomial corrections around discontinuities. 
 * The PolyBLEP technique uses polynomial residuals to remove aliasing artifacts
 * while maintaining the rich harmonic content of sawtooth waves.
 *
 * Usage:
 * - Call setFrequency() to set the oscillator frequency
 * - Call setSampleRate() during initialization or when sample rate changes
 * - The processBlock() method generates band-limited sawtooth samples
 * 
 * Background:
 * PolyBLEP provides efficient anti-aliasing by detecting phase discontinuities
 * and applying 2nd-order polynomial corrections only when needed, preserving
 * computational efficiency while eliminating aliasing artifacts.
 * 
 * Example:
 * @code
 * PolyBLEP osc;
 * osc.setSampleRate(48000.0f);
 * osc.setFrequency(440.0f);
 * osc.setAmplitude(0.8f);
 * // Process audio blocks - outputs will be available in connected bus
 * @endcode
 */
class PolyBLEP : public SoundProcessor
{
public:
    /**
     * @brief Constructor - initializes PolyBLEP oscillator
     */
    PolyBLEP();
    
    /**
     * @brief Destructor
     */
    ~PolyBLEP();

    /**
     * @brief Sets the oscillator frequency in Hz
     * 
     * @param freq Frequency in Hz
     */
    void setFrequency(host_float freq);

    /**
     * @brief Resets the oscillator phase to zero
     */
    void reset();

protected:
    /**
     * @brief Initializes the DSP object with default values
     */
    void initializeProcessor() override;

private:
    /**
     * @brief Static dispatcher for DSP block processing
     * 
     * @param dsp Pointer to the DSPObject instance
     */
    static void processBlock(DSPObject *dsp);

    /**
     * @brief Internal block processing method
     * 
     * Generates band-limited sawtooth samples and outputs to connected audio bus.
     */
    void processBlock();

    /**
     * @brief Calculates the PolyBLEP correction for phase discontinuities
     * 
     * @param t Normalized phase offset from discontinuity (-1.0 to 1.0)
     * @return PolyBLEP correction value
     */
    inline host_float polyBlep(host_float t);

    /// @brief Current phase accumulator (0.0 to 1.0)
    host_float phase;
    
    /// @brief Phase increment per sample
    host_float phaseIncrement;
    
    /// @brief Current frequency in Hz
    host_float frequency;
};