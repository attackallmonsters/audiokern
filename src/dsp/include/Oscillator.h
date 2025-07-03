#pragma once

#include <cmath>
#include <memory>
#include "clamp.h"
#include "DSP.h"
#include "DSPObject.h"
#include "DSPBuffer.h"
#include "VoiceOptions.h"
#include "dsp_types.h"
#include "dsp_math.h"

// FM frequency calculation
using FMCalcFunc = dsp_float (*)(const dsp_float &baseFreq, const dsp_float &modulator, const dsp_float &index);

// Abstract base class for all oscillator types.
// This class provides a common interface and shared internal phase state
// for generating periodic waveforms based on frequency and sample rate.
// Concrete subclasses must implement their specific waveform behavior.
class Oscillator : public DSPObject
{
public:
    // Oscillator is initialized with current phase
    Oscillator();

    // Virtual destructor to allow proper cleanup in derived classes
    virtual ~Oscillator();

    // Initializes the oscillator
    void initialize() override;

    // Sets the detune factor
    virtual void setDetune(dsp_float /*value*/){};

    // Sets the duty cycle for PWM
    virtual void setDutyCycle(dsp_float /*value*/){};

    // Resets the internal oscillator phase to 0.0.
    virtual void resetPhase();

    // Sets the number of voices
    virtual void setNumVoices(int /*count*/) {};

    // Sets the desired oscillator frequency in Hertz
    void setFrequency(dsp_float value);

    // Sets the pitch offset in semi tones
    void setPitchOffset(int value);

    // Sets the fine tuning in cent
    void setFineTune(dsp_float value);

    // Enables negative phase wrapping
    void setNegativeWrappingEnabled(bool enabled);

    // Calculates the effective frequency based on base frequency,
    // pitch offset (in semitones), and fine-tuning (in cents).
    // Then updates the phase increment accordingly.
    void setCalculatedFrequency(dsp_float f);

    // Gets the current frequency
    dsp_float getFrequency();

    // Gets the calculated frequency (base + pitchOffset + finetune))
    dsp_float getCalculatedFrequency();

    // Sets the type of FM to use
    void setFMType(FMType fm);

    // Sets the modulation index for frequency modulation.
    // This controls the intensity of the frequency modulation effect.
    void setModIndex(dsp_float index);

    // Returns true if the oscillator's phase wrapped during the last getSample() call
    bool hasWrapped();

    // Resets the wrap status
    void unWrap();

    // Buffer for modulation
    DSPBuffer modBufferL;
    DSPBuffer modBufferR;

    // Sample buffer for output
    DSPBuffer outBufferL;
    DSPBuffer outBufferR;

protected:
    bool syncEnabled;                    // Enables or disable block wise phase synchronization
    bool negativeWrappingEnabled = true; // Indicates if negative phase wrapping is enabled
    dsp_float frequency;                 // The desired oscillator frequency in Hertz
    dsp_float calculatedFrequency;       // The calculated FM frequency in Hertz
    int pitchOffset;                     // offset in half tones
    dsp_float fineTune;                  // fine tune in cent
    dsp_float phaseIncrement;            // Increment based on frquency and sample rate
    dsp_float currentPhase;              // Current phase of the oscillator in radians [0, 2π]
    bool wrapped = false;                // True when phase wrapped

    FMType fmType = FMType::ThroughZero; // The FM operation mode
    dsp_float modulationIndex = 0;       // FM depth: how much modulator modulates carrier

    // Avoid vtable lookup for sample calculation
    using SampleGenerator = void (*)(
        Oscillator * /*osc*/, 
        const dsp_float & /*frequency*/, 
        const dsp_float & /*phase*/, 
        dsp_float & /*left*/, 
        dsp_float & /*right*/, 
        const dsp_float & /*modLeft*/, 
        const dsp_float & /*modRight*/);

    // Derived classes registers sample generator
    void registerSampleGenerator(SampleGenerator sg);

private:
    FMCalcFunc fmFunc;                  // FM calculation method
    SampleGenerator generateSampleFunc; // Sample generation ba derived class

    // Next sample block generation
    static void processBlock(DSPObject *dsp);

    // Dummy ComputeSampleFunc for setSamples
    static void generateSample(Oscillator * /*osc*/, 
        const dsp_float & /*frequency*/, 
        const dsp_float & /*phase*/, 
        dsp_float & /*left*/, 
        dsp_float & /*right*/, 
        const dsp_float & /*modLeft*/, 
        const dsp_float & /*modRight*/);
};
