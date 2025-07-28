#pragma once

#include "Modulator.h"
#include "DSPSampleBuffer.h"
#include "dsp_types.h"
#include "dsp_math.h"
#include "clamp.h"
#include <cmath>
#include <cstdlib>
#include <functional>

/**
 * @brief Available LFO waveform types.
 */
enum class LFOType
{
    Sine,
    RampUp,
    RampDown,
    Triangle,
    Square,
    Random
};

/**
 * @brief LFO operating modes.
 * - Buffered: Fills a full audio-rate buffer each block.
 * - Value: Computes only one value per block (e.g., for param control).
 */
enum class LFOMode
{
    Buffered,
    Value
};

/**
 * @brief Low-Frequency Oscillator class for audio modulation.
 *
 * This class supports multiple waveform types, waveform shaping, unipolar/bipolar mode,
 * smoothing, phase wrapping, pulse width modulation, and FM-style depth modulation.
 *
 * The output is either a full audio buffer (`Buffered` mode) or a single value per block (`Value` mode).
 * When phase wraps around, an optional callback (`onPhaseWrap`) can be triggered.
 */
class LFO : public Modulator
{
public:
    // Constructor: sets default parameters
    LFO();

        /**
     * @brief Sets the frequency of the LFO in Hertz.
     * 
     * Controls how many cycles per second the waveform completes.
     * Must be > 0.0 to avoid invalid phase increments.
     *
     * @param f Frequency in Hz (typically 0.01 – 20.0)
     */
    void setFreq(host_float f);

    /**
     * @brief Sets the waveform type of the LFO.
     *
     * Determines the shape of the modulation signal.
     * Supported types: Sine, RampUp, RampDown, Triangle, Square, Random.
     *
     * @param type The desired waveform type
     */
    void setType(LFOType type);

    /**
     * @brief Sets the DC offset added to the LFO output.
     *
     * Offset is added after all other processing steps (including depth).
     * This allows shifting the entire waveform up or down.
     *
     * @param f Offset value (usually in range -1.0 to 1.0)
     */
    void setOffset(host_float f);

    /**
     * @brief Sets the depth (amplitude) of the LFO output.
     *
     * Affects how far the waveform swings above and below zero (or 0.5 in unipolar mode).
     * Values outside [0.0, 1.0] are allowed but may clip downstream.
     *
     * @param f Depth multiplier (0.0 = no modulation, 1.0 = full amplitude)
     */
    void setDepth(host_float f);

    /**
     * @brief Sets the shape amount for ramp-based waveforms.
     *
     * Shapes the linear ramp curve into exponential/logarithmic curves.
     * 
     * - 0.0 = linear ramp  
     * - < 0.0 = convex (log-like)  
     * - > 0.0 = concave (exp-like)
     *
     * Has no effect on sine or square waveforms.
     *
     * @param f Shape factor (e.g., -5.0 to +5.0)
     */
    void setShape(host_float f);

    /**
     * @brief Sets the pulse width for the square waveform.
     *
     * Only affects square wave output. Defines the proportion of time the output is "high".
     *
     * @param f Pulse width as a fraction (0.0 = always low, 0.5 = symmetric, 1.0 = always high)
     */
    void setPulseWidth(host_float f);

    /**
     * @brief Sets the smoothing factor for the output.
     *
     * Applies a 1-pole lowpass filter to reduce sharp transitions in the waveform.
     * Helps avoid audio clicks during fast modulation.
     *
     * @param f Smoothing factor (0.0 = no smoothing, 0.99 = strong smoothing)
     */
    void setSmooth(host_float f);

    /**
     * @brief Sets the fallback signal value when LFO is idle or disabled.
     *
     * This value is used as output when no waveform is active or phase reset prevents output.
     *
     * @param f Static fallback value (typically 0.0 or 0.5)
     */
    void setIdleSignal(host_float f);

    /**
     * @brief Enables or disables unipolar output mode.
     *
     * - false = Bipolar mode: output ranges from -1.0 to +1.0  
     * - true  = Unipolar mode: output ranges from 0.0 to +1.0
     *
     * @param enabled True for unipolar, false for bipolar
     */
    void isUnipolar(bool enabled);

    /**
     * @brief Sets the LFO operating mode.
     *
     * Determines how the LFO is processed:
     * - `Buffered`: generates a full audio-rate buffer
     * - `Value`: computes a single control value per block
     *
     * @param mode Operating mode
     */
    void setMode(LFOMode mode);


    /** 
     * @brief Reset internal oscillator phase
     */
    void reset();

    /**
     * @brief Optional callback triggered when the LFO phase wraps around.
     * Can be used for tempo sync or retrigger behavior.
     */
    std::function<void()> onPhaseWrap = nullptr;

    /**
     * @brief Optional callback with the first value of each block in Value mode.
     * Useful for per-block parameter modulation.
     */
    std::function<void(host_float)> processLFOValue = nullptr;

    /**
     * @brief Buffer for audio-rate LFO output.
     * Only used in Buffered mode.
     */
    DSPSampleBuffer modulationBuffer;

protected:
    // Called once during DSP setup to initialize internal state
    void initializeModulator() override;

private:
    // Static dispatcher for Buffered mode
    static void processBlockBuffer(DSPObject *dsp);

    // Static dispatcher for Value mode
    static void processBlockValue(DSPObject *dsp);

    // Internal block processing for Buffered mode
    void processBlockBuffer();

    // Internal block processing for Value mode
    void processBlockValue();

    // Internal phase and frequency variables
    host_float phase;        // Current oscillator phase [0.0 ... 1.0)
    host_float freq;         // Frequency in Hz
    host_float phaseInc;     // Phase increment per sample

    // LFO parameter values
    host_float offset;       // DC offset added to output
    host_float depth;        // Modulation depth (amplitude)
    host_float shape;        // Shape factor (nonlinear ramp shaping)
    host_float pw;           // Pulse width (for square)
    host_float smoothVal;    // Current smoothed output value
    host_float smoothCoeff;  // Smoothing coefficient
    host_float idleSignal;   // Fallback value when idle
    bool unipolar;           // Output mode: true = [0..1], false = [-1..1]
    LFOMode lfoMode;         // Operation mode (buffer or per-block value)

    LFOType lfoType;         // Current waveform type

    // Apply shaping to ramp waveform
    host_float shapedRamp(host_float x);

    // Waveform generation functions
    host_float lfoSine();
    host_float lfoRampUp();
    host_float lfoRampDown();
    host_float lfoTriangle();
    host_float lfoSquare();
    host_float lfoRandom();

    // Flag indicating if frequency modulation (FM) is enabled
    bool fmEnabled;

    // Pointer to the current waveform function
    host_float (LFO::*lfoFunc)() = nullptr;
};
