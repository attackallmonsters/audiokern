#pragma once

#include "DSPObject.h"
#include "XDSPBuffer.h"
#include "DSPSampleBuffer.h"
#include "dsp_math.h"
#include "clamp.h"
#include <vector>
#include <cmath>
#include <memory>
#include <fstream>

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
    // Initializes all wavetable buffers for multiple frequency ranges
    void initialize() override;

    // Sets the number of voices
    void setNumVoices(int count);

    // Sets the detune factor for the voices
    void setDetune(dsp_float value);

    // Sets the desired oscillator frequency in Hertz
    void setFrequency(dsp_float value);

    // Sets the pitch offset in semi tones
    void setPitchOffset(int value);

    // Sets the fine tuning in cent
    void setFineTune(dsp_float value);

    // Gets the current frequency
    dsp_float getFrequency();

    // Gets the calculated frequency (base + pitchOffset + finetune))
    dsp_float getCalculatedFrequency();

    // Sets the modulation index for frequency modulation.
    // This controls the intensity of the frequency modulation effect.
    void setModIndex(dsp_float index);

    // Returns true if the oscillator's phase wrapped during the last getSample() call
    bool hasWrapped();

    // Resets the wrap status
    void unWrap();

    // Resets the internal oscillator phase to 0.0.
    void resetPhase();

    // Buffer for modulation
    DSPSampleBuffer modulationBufferL;
    DSPSampleBuffer modulationBufferR;

    // Sample buffer for output
    DSPSampleBuffer outputBufferL;
    DSPSampleBuffer outputBufferR;

protected:
    // Ctor: expects an unique name for the waveform
    WavetableOscillator(const std::string formName);

    // Each subclass must define how to fill a table
    virtual void createWavetable(XDSPBuffer &buffer, dsp_float frequency) = 0;

    // Frequency boundaries per LUT (e.g. per octave)
    std::vector<double> baseFrequencies;

    // Define the corresponding table size for each frequency range
    std::vector<size_t> tableSizes;

    // One wavetable buffer per frequency band
    std::vector<std::unique_ptr<XDSPBuffer>> wavetableBuffers;

private:
    // Next sample block generation
    static void processBlock(DSPObject *dsp);

    // Calculates the effective frequency based on base frequency,
    // pitch offset (in semitones), and fine-tuning (in cents).
    // Then updates the phase increment accordingly.
    void setCalculatedFrequency(dsp_float f);

    // Loads a wavetable
    bool load();

    // Saves a wavetable
    void save() const;

    // Select appropriate wavetable for the given frequency
    void selectTable(double frequency);

    // Update detune on voices
    void updateDetune();

    // The waveform name
    std::string waveformName;

    // stores the last wavetable to prevent lookup when frequency did not change
    const XDSPBuffer *selectedWaveTable = nullptr;
    size_t selectedWaveTableSize = 0;
    dsp_float lastFrequency = -1.0;

    // The number of voices
    int numVoices = 1;
    std::vector<WavetableVoice> voices;

    // Voices detune
    dsp_float detune = 0.03;

    dsp_float frequency;           // The desired oscillator frequency in Hertz
    dsp_float calculatedFrequency; // The calculated FM frequency in Hertz
    int pitchOffset;               // offset in half tones
    dsp_float fineTune;            // fine tune in cent
    dsp_float modulationIndex = 0; // Phase modulation depth: how much modulator modulates phase of carrier
    dsp_float phaseIncrement;      // Increment based on frquency and sample rate
    dsp_float currentPhase;        // Current phase of the oscillator in radians [0, 2π]
    bool wrapped = false;          // True when phase wrapped
};
