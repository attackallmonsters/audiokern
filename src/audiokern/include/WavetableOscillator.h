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
 */
struct SharedWavetableSet {
    std::string name;
    std::vector<host_float> baseFrequencies;
    std::vector<size_t> tableSizes;
    std::vector<DSPSampleBuffer*> buffers;
};

/**
 * @brief Internal representation of a single oscillator voice with detune and stereo gain.
 */
struct WavetableVoice
{
    dsp_float phase;
    dsp_float detune_ratio;
    dsp_float amp_ratio;
    dsp_float gainL;
    dsp_float gainR;
};

/**
 * @brief Abstract base class for all wavetable-based oscillator generators.
 * 
 * Each subclass must implement createWavetable(...) to generate its waveform content.
 * The oscillator supports polyphony via multiple detuned voices.
 * 
 * Upon initialization, the object automatically registers its own modulation bus
 * using its instance name (via getName()), inherited from SoundGenerator.
 */
class WavetableOscillator : public SoundGenerator
{
public:
    /// Virtual destructor releases all allocated buffers
    ~WavetableOscillator();

    /**
     * @brief Sets the number of voices used for unison (e.g., for Supersaw).
     * 
     * @param count Number of detuned voices (>= 1)
     */
    void setNumVoices(int count);

    /**
     * @brief Sets the detune factor for spreading voices in frequency.
     * 
     * @param value Detune ratio (0.0 = no detune)
     */
    void setDetune(dsp_float value);

    /**
     * @brief Sets the oscillator's base frequency in Hz.
     * 
     * @param value Frequency in Hz
     */
    void setFrequency(dsp_float value);

    /// Returns the currently set oscillator frequency in Hz
    dsp_float getFrequency();

    /**
     * @brief Sets the modulation index used for phase modulation (FM).
     * 
     * @param index Modulation index (0.0 = no modulation)
     */
    void setModIndex(dsp_float index);

    /// Returns true if the oscillator's phase wrapped during the last sample cycle
    bool hasWrapped();

    /// Resets the wrapped-phase flag (should be called after detection)
    void unWrap();

    /// Resets the internal oscillator phase to 0.0
    void resetPhase();    

protected:
    /**
     * @brief Called by base class after DSP system is initialized.
     * 
     * Connects audio buses and loads or generates required wavetable content.
     */
    void initializeGenerator() override;

    /**
     * @brief Protected constructor.
     * 
     * Subclasses must specify the waveform's unique name (used for shared caching).
     * 
     * @param formName The waveform name for wavetable identification
     */
    WavetableOscillator(const std::string formName);

    /**
     * @brief Pure virtual function to define how the wavetable is generated.
     * 
     * Must be implemented by concrete subclasses to fill the buffer with one waveform cycle.
     * 
     * @param buffer Output buffer to fill
     * @param frequency Target base frequency of the waveform
     */
    virtual void createWavetable(DSPBuffer& buffer, dsp_float frequency) = 0;

    /// List of frequency boundaries used to select wavetable variants
    std::vector<host_float> baseFrequencies;

    /// Corresponding wavetable size for each frequency band
    std::vector<size_t> tableSizes;

private:
    /// Static block-based wrapper for mono voice processing
    static void processBlockVoice(DSPObject* dsp);

    /// Static block-based wrapper for polyphonic processing
    static void processBlockVoices(DSPObject* dsp);

    /// Processes a single block of audio for one voice
    void processBlockVoice();

    /// Processes a single block of audio for all voices (polyphonic)
    void processBlockVoices();

    /// Loads an existing wavetable set from file
    bool load();

    /// Saves the generated wavetable set to file
    void save() const;

    /// Selects the appropriate wavetable based on current frequency
    void selectTable(double frequency);

    /// Recomputes voice detune settings
    void updateDetune();

    /// Computes gain scaling based on number of voices
    dsp_float getVocieGain(int numVoices);

    /// Name used to identify shared wavetables
    std::string waveformName;

    /// Pointer to last selected wavetable
    const DSPSampleBuffer* selectedWaveTable = nullptr;

    /// Number of samples in selected table
    size_t selectedWaveTableSize = 0;

    /// Last frequency used for table selection
    dsp_float lastFrequency = -1.0;

    /// Number of voices
    int numVoices = 1;

    /// List of voice state objects
    std::vector<WavetableVoice> voices;

    /// Temporary high-precision buffers for wavetable generation
    std::vector<DSPBuffer*> wavetableCalcBuffers;

    /// Final resampled wavetables for runtime use
    std::vector<DSPSampleBuffer*> wavetableSampleBuffers;

    /// Current detune factor
    dsp_float detune = 0.03;

    /// Core frequency, pitch and FM control
    dsp_float frequency;
    int pitchOffset;
    dsp_float fineTune;
    dsp_float modulationIndex = 0;

    /// Phase tracking
    dsp_float phaseIncrement;
    dsp_float currentPhase;
    bool wrapped = false;

    /// Gain factor per voice
    dsp_float voiceGain;

    /// Shared wavetable memory across multiple instances
    static std::vector<SharedWavetableSet> sharedWavetables;

    /// Attempts to acquire precomputed wavetable set from shared cache
    void acquireSharedWavetable();
};
