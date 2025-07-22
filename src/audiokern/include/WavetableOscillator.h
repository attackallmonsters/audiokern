#pragma once

#include "DSPObject.h"
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

struct SharedWavetableSet {
    std::string name;
    std::vector<host_float> baseFrequencies;
    std::vector<size_t> tableSizes;
    std::vector<DSPSampleBuffer*> buffers;
};

// Internal voice struct used for detuned oscillators
struct WavetableVoice
{
    dsp_float phase;
    dsp_float detune_ratio;
    dsp_float amp_ratio;
    dsp_float gainL;
    dsp_float gainR;
};

// Abstract base class for all wavetable-based oscillators
class WavetableOscillator : public DSPObject
{
public:
    // Destructor releases memory for wavetables
    ~WavetableOscillator();
    
    // Sets the number of voices
    void setNumVoices(int count);

    // Sets the detune factor for the voices
    void setDetune(dsp_float value);

    // Sets the oscillation frequency
    void setFrequency(dsp_float value);

    // Gets the current frequency
    dsp_float getFrequency();

    // Sets the modulation index for frequency modulation.
    // This controls the intensity of the frequency modulation effect.
    void setModIndex(dsp_float index);

    // Returns true if the oscillator's phase wrapped during the last getSample() call
    bool hasWrapped();

    // Resets the wrap status
    void unWrap();

    // Resets the internal oscillator phase to 0.0.
    void resetPhase();

    // Next sample block generation one voice
    void processBlockVoice();

    // Next sample block generation multiple voices
    void processBlockVoices();

protected:
    // Initializes all wavetable buffers for multiple frequency ranges
    DSPUsage initializeObject() override;

    // Ctor: expects an unique name for the waveform
    WavetableOscillator(const std::string formName);

    // Each subclass must define how to fill a table
    virtual void createWavetable(DSPBuffer &buffer, dsp_float frequency) = 0;

    // Frequency boundaries per LUT (e.g. per octave)
    std::vector<host_float> baseFrequencies;

    // Define the corresponding table size for each frequency range
    std::vector<size_t> tableSizes;   

private:
    // Next sample block generation one voice
    static void processBlockVoice(DSPObject *dsp);

    // Next sample block generation multiple voices
    static void processBlockVoices(DSPObject *dsp);

    // Loads a wavetable
    bool load();

    // Saves a wavetable
    void save() const;

    // Select appropriate wavetable for the given frequency
    void selectTable(double frequency);

    // Update detune on voices
    void updateDetune();

    // Adjusts the volume to the number of voices
    dsp_float getVocieGain(int numVoices);

    // The waveform name
    std::string waveformName;

    // stores the last wavetable to prevent lookup when frequency did not change
    const DSPSampleBuffer *selectedWaveTable = nullptr;
    size_t selectedWaveTableSize = 0;
    dsp_float lastFrequency = -1.0;

    // The number of voices
    int numVoices = 1;
    std::vector<WavetableVoice> voices;

    // High-precision wavetable buffers
    std::vector<DSPBuffer*> wavetableCalcBuffers;

    // Wavetable sample buffers
    std::vector<DSPSampleBuffer*> wavetableSampleBuffers;

    // Voices detune
    dsp_float detune = 0.03;

    dsp_float frequency;           // The desired oscillator frequency in Hertz
    int pitchOffset;               // offset in half tones
    dsp_float fineTune;            // fine tune in cent
    dsp_float modulationIndex = 0; // Phase modulation depth: how much modulator modulates phase of carrier
    dsp_float phaseIncrement;      // Increment based on frquency and sample rate
    dsp_float currentPhase;        // Current phase of the oscillator in radians [0, 2π]
    bool wrapped = false;          // True when phase wrapped
    dsp_float voiceGain;           // The gain of a single voice


    static std::vector<SharedWavetableSet> sharedWavetables;
    void acquireSharedWavetable();
};
