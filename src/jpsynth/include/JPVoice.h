#pragma once

#include "ParamFader.h"
#include "VoiceOptions.h"
#include "NoiseGenerator.h"
#include "SineWavetable.h"
#include "SawWavetable.h"
#include "TriangleWavetable.h"
#include "SquareWavetable.h"
#include "HarmonicClusterWavetable.h"
#include "FibonacciWavetable.h"
#include "MirrorWavetable.h"
#include "ModuloWavetable.h"
#include "BitWavetable.h"
#include "KorgonFilter.h"
#include "DSP.h"
#include "SoundGenerator.h"
#include "dsp_types.h"
#include "dsp_math.h"
#include "ADSR.h"
#include "clamp.h"
#include <cmath>
#include "DSPSampleBuffer.h"

struct ADSRParams
{
    host_float attackTime;
    host_float decayTime;
    host_float sustainLevel;
    host_float releaseTime;
    host_float attackShape;
    host_float releaseShape;
};

// The Voice class manages two oscillators and handles interactions like
// Frequency Modulation (FM) and Oscillator Sync between them.
class JPVoice : public SoundGenerator
{
public:
    // Constructor: requires two Oscillator pointers and the global sample rateSample
    JPVoice();

    // Destructor: deletes both oscillator instances
    ~JPVoice();

    // Start ADSRs
    void playNote();

    // Stop  ADSRs
    void stopNote();

    // Sets the modulation index for frequency modulation.
    // This controls the intensity of the frequency modulation effect.
    void setModIndex(host_float index);

    // Enables or disables oscillator sync
    void setSyncEnabled(bool enabled);

    // Sets the frequency of oscillator 1/carrier
    void setCarrierFrequency(host_float f);

    // Sets the frequency of oscillator 2/modulator
    void setModulatorFrequency(host_float f);

    // Sets the number of voices
    void setNumVoices(int count);

    // Sets the volume level of the oscillators
    void setOscillatorMix(host_float mix);

    // Sets the volume level of the noise generator
    void setNoiseMix(host_float mix);

    // Assigns the carrier oscillator
    void setCarrierOscillatorType(CarrierOscillatiorType oscillatorType);

    // Assigns the modulator oscillator
    void setModulatorOscillatorType(ModulatorOscillatorType oscillatorType);

    // Changes the current noise type (white or pink)
    void setNoiseType(NoiseType type);

    // Sets the detune factor
    void setDetune(host_float value);

    // Sets the feedback amount for the carrier
    void setFeedbackCarrier(host_float feedback);

    // Sets the feedback amount for the modulator
    void setFeedbackModulator(host_float feedback);

    // Sets the filter type
    void setFilterMode(FilterMode mode);

    // Sets the cutoff frequency
    void setFilterCutoff(host_float f);

    // Sets the filter resonance
    void setFilterResonance(host_float r);

    // Sets the filter drive
    void setFilterDrive(host_float value);

    // Filter envelope params
    void setFilterADSR(ADSRParams &params);

    // Amplification envelope params
    void setAmpADSR(ADSRParams &params);

    // Link the ADSRs
    void linkADSR(bool isEnabled);

    // Set adrss to one shot mode
    void setADSROneshot(bool isEnabled);

    // Sets the amp envelope gain
    void setAmpGain(host_float g);

    /** @brief Sets the analog feeling (amound) */
    void setAnalogDrift(host_float amount);

    /** @brief Sets the modulation bus for filter citoff */
    void setFilterCutoffModulationBus(DSPModulationBus &bus);

    // Next sample block generation
    void processBlock();

protected:
    // Initializes the DSP object
    void initializeGenerator() override;

    void onOutputBusConnected(DSPAudioBus &bus) override;

private:
    // Next sample block generation
    static void processBlock(DSPObject *dsp);

    WavetableOscillator *carrier;      // Carrier oscillator (may be modulated)
    WavetableOscillator *modulator;    // Modulator oscillator (for FM or sync)
    WavetableOscillator *carrierTmp;   // Carrier oscillator for oscillator change
    WavetableOscillator *modulatorTmp; // Modulator oscillator for oscillator change

    host_float carrierFrequency = 0.0;   // Current frequency carrier
    host_float modulatorFrequency = 0.0; // Current frequency modulator

    host_float modulationIndex = 0; // FM depth: how much modulator modulates carrier

    host_float oscmix = 0.0;   // Mix carrier <=> modulator
    host_float noisemix = 0.0; // Mix oscillators <=> noise
    bool syncEnabled = false;  // True if Sync is active

    host_float filterCutoff;    ///< filter cutof frequency
    host_float filterResonance; // filter reso

    host_float detune = 0.0;     // Detune factor supersaw oszillator
    host_float pulseWidth = 0.5; // Pulse width square oscillator

    // Oscillators
    NoiseGenerator noise;
    SineWavetable sineCarrier;
    SineWavetable sineModulator;
    SawWavetable sawCarrier;
    SawWavetable sawModulator;
    SquareWavetable squareCarrier;
    SquareWavetable squareModulator;
    TriangleWavetable trianlgeCarrier;
    TriangleWavetable triangleModulator;
    HarmonicClusterWavetable clusterCarrier;
    HarmonicClusterWavetable clusterModulator;
    FibonacciWavetable fibonacciCarrier;
    FibonacciWavetable fibonacciModulator;
    MirrorWavetable mirrorCarrier;
    MirrorWavetable mirrorModulator;
    ModuloWavetable moduloCarrier;
    ModuloWavetable moduloModulator;
    BitWavetable bitModulator;

    host_float oscDrift;

    // Audio buffers
    DSPAudioBus carrierAudioBus;
    DSPAudioBus modulatorAudioBus;
    DSPAudioBus noiseAudioBus;
    DSPModulationBus filterCutoffBus;
    DSPModulationBus filterCutoffModulationBus;
    DSPModulationBus outputAmplificationBus;

    std::string carrierAudioBusName;
    std::string modulatorAudioBusName;
    std::string noiseAudioBusName;
    std::string filterCutoffBusName;
    std::string outputAmplificationBusName;

    // Multi mode filter
    KorgonFilter filter;

    // Modulation objects
    ADSR filterAdsr;
    ADSR ampAdsr;
    bool adsrLinked;
    void setFilterADSRLink(ADSRParams &params, bool setOther);
    void setAmpADSRLink(ADSRParams &params, bool setOther);

    // DSP working vars
    host_float carrierLeft, carrierRight;
    host_float modLeft, modRight;
    host_float mixL, mixR;
    host_float amp_carrier;
    host_float amp_modulator;
    host_float amp_oscmix;
    host_float amp_oscs;
    host_float amp_noise;

    // Feedback
    host_float lastSampleCarrierLeft;
    host_float lastSampleCarrierRight;
    host_float lastSampleModulatorLeft;
    host_float lastSampleModulatorRight;
    host_float feedbackAmountCarrier = 0.0;
    host_float feedbackAmountModulator = 0.0;

    // Number of voices
    int numVoices = 1;

    // Parameter change fader
    ParamFader paramFader;
};
