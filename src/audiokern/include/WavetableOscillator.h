#pragma once

#include "SoundGenerator.h"
#include "DSPBuffer.h"
#include "DSPSampleBuffer.h"
#include "dsp_math.h"
#include "clamp.h"

#include <vector>
#include <cmath>
#include <memory>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <limits.h>
#include <cstdlib>

class DSPObject; // Forward declaration

/**
 * @brief Struct holding a shared wavetable set for reuse between oscillator instances.
 *
 * This structure is used to cache wavetable data across multiple oscillator instances
 * to avoid redundant recalculation or file loading. Tables are identified by a waveform name.
 */
struct SharedWavetableSet
{
    std::string name;
    std::vector<host_float> baseFrequencies;
    std::vector<size_t> tableSizes;
    std::vector<DSPSampleBuffer *> buffers;
};

/**
 * @brief Internal representation of a single oscillator voice with detune and stereo gain.
 *
 * Each WavetableVoice defines the phase state, detune offset, amplitude scaling, and stereo panning
 * used in polyphonic or unison-mode oscillator operation.
 */
struct WavetableVoice
{
    host_float phase;        ///< Current oscillator phase [0..1)
    host_float detune_ratio; ///< Detune ratio relative to base frequency
    host_float amp_ratio;    ///< Amplitude weight for stereo spread
    host_float gainL;        ///< Gain applied to left channel
    host_float gainR;        ///< Gain applied to right channel
};

/**
 * @brief Abstract base class for all wavetable-based oscillator generators.
 *
 * Provides core functionality for polyphonic or unison oscillator generation using
 * precomputed or dynamically generated wavetables. Supports per-voice detuning,
 * FM modulation, analog-style drift, and shared wavetable memory.
 *
 * Usage:
 * - Derive a concrete oscillator (e.g. `SineWavetable`, `SawWavetable`) and implement `createWavetable(...)`.
 * - Use `setFrequency()` and `setNumVoices()` to configure.
 * - Call `initialize()` once sample rate and block size are known.
 * - Connect the oscillator via `connectAudioOut(...)`.
 */
class WavetableOscillator : public SoundGenerator
{
public:
    /// Virtual destructor releases all allocated buffers
    ~WavetableOscillator();

    /**
     * @brief Sets the number of voices used for unison (e.g., for Supersaw).
     *
     * Re-initializes internal voice buffers. Must be ≥ 1.
     * Can be called at runtime to change number of stacked voices.
     *
     * @param count Number of detuned voices (≥ 1)
     */
    void setNumVoices(int count);

    /**
     * @brief Sets the detune factor for spreading voices in frequency.
     *
     * The detune factor defines the spacing between unison voices in semitone ratio.
     * For example, a value of 0.03 results in roughly ±3% spread per voice.
     *
     * @param value Detune ratio (0.0 = no detune)
     */
    void setDetune(host_float value);

    /**
     * @brief Sets the oscillator's base frequency in Hz.
     *
     * The internal table selection and phase increment are recomputed automatically.
     *
     * @param value Frequency in Hz
     */
    void setFrequency(host_float value);

    /**
     * @brief Returns the currently set oscillator frequency in Hz.
     */
    host_float getFrequency();

    /**
     * @brief Sets the modulation index used for phase modulation (FM).
     *
     * When non-zero, the oscillator's phase is modulated by the input signal.
     *
     * @param index Modulation index (0.0 = no modulation)
     */
    void setModIndex(host_float index);

    /**
     * @brief Returns true if the oscillator's phase wrapped during the last sample cycle.
     *
     * Useful for synchronizing events (e.g. triggering envelopes on wave wraparound).
     */
    bool hasWrapped();

    /**
     * @brief Resets the wrapped-phase flag.
     *
     * Should be called once after checking `hasWrapped()` to avoid stale state.
     */
    void unWrap();

    /**
     * @brief Resets the internal oscillator phase to 0.0.
     */
    void resetPhase();

    /**
     * @brief Enables analog-style pitch drift for oscillator output.
     *
     * Applies a subtle, time-varying detuning to simulate analog oscillator instability.
     * The drift value defines the maximum frequency deviation in semitone units.
     * A value of 0.0 disables drift; typical useful values are in the range 0.01–0.1.
     *
     * Internally, the drift is applied per audio block using a smoothed random walk.
     * The detuned frequency is computed as:
     *
     *   frequency = baseFrequency * 2^(driftValue)
     *
     * @param d Pitch drift in semitones (e.g. 0.05 = ±5 cents)
     */
    void setAnalogDrift(host_float d);

protected:
    /**
     * @brief Called by base class after DSP system is initialized.
     *
     * Loads or generates the required wavetable content, selects appropriate tables,
     * and initializes output bus connections.
     */
    void initializeGenerator() override;

    /**
     * @brief Protected constructor.
     *
     * Subclasses must provide the waveform's name for cache identification and
     * file-based loading/saving.
     *
     * @param formName The waveform name for wavetable identification
     */
    WavetableOscillator(const std::string formName);

    /**
     * @brief Pure virtual function to define how the wavetable is generated.
     *
     * Must be implemented by concrete subclasses to fill the buffer with one waveform cycle.
     *
     * @param buffer Output buffer to fill (high precision)
     * @param frequency Target base frequency of the waveform
     */
    virtual void createWavetable(DSPBuffer &buffer, dsp_float frequency) = 0;

    /// List of frequency boundaries used to select wavetable variants
    std::vector<host_float> baseFrequencies;

    /// Corresponding wavetable size for each frequency band
    std::vector<size_t> tableSizes;

private:
    // === Internal processing ===

    /// Static block-based wrapper for mono voice processing
    static void processBlockVoice(DSPObject *dsp);

    /// Static block-based wrapper for polyphonic processing
    static void processBlockVoices(DSPObject *dsp);

    /// Processes a single block of audio for one voice
    void processBlockVoice();

    /// Processes a single block of audio for all voices (polyphonic)
    void processBlockVoices();

    // === Wavetable generation and selection ===

    /// Loads an existing wavetable set from file
    bool load();

    /// Saves the generated wavetable set to file
    void save() const;

    /// Selects the appropriate wavetable based on current frequency
    void selectTable(double frequency);

    /// Recomputes voice detune settings
    void updateDetune();

    /// Computes gain scaling based on number of voices
    host_float getVocieGain(int numVoices);

    /// Attempts to acquire precomputed wavetable set from shared cache
    void acquireSharedWavetable();

    // === Data ===

    std::string waveformName; ///< Unique name for this waveform

    const DSPSampleBuffer *selectedWaveTable; ///< Pointer to currently selected table
    size_t selectedWaveTableSize;             ///< Number of samples in selected table
    host_float lastFrequency;                  ///< Last used frequency for table selection

    int numVoices = 1;                  ///< Number of detuned voices
    std::vector<WavetableVoice> voices; ///< Per-voice phase and detune states

    host_float detune = 0.03;         ///< Detune spread factor
    host_float frequency = 440.0;     ///< Core oscillator frequency
    int pitchOffset = 0;             ///< Optional MIDI pitch shift
    host_float fineTune = 0.0;        ///< Fine tuning adjustment
    host_float modulationIndex = 0.0; ///< FM depth (mod index)

    host_float phaseIncrement = 0.0; ///< Computed per-block increment
    host_float currentPhase = 0.0;   ///< Current phase accumulator
    bool wrapped = false;           ///< Phase wrap detection

    host_float voiceGain = 1.0; ///< Per-voice amplitude compensation

    host_float drift; ///< Optional analog-style drift

    static std::vector<SharedWavetableSet> sharedWavetables; ///< Global shared cache
    std::vector<DSPBuffer *> wavetableCalcBuffers;           ///< Temp buffer for table generation
    std::vector<DSPSampleBuffer *> wavetableSampleBuffers;   ///< Final runtime wavetable data
};
