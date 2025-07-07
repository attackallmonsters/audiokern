#pragma once

#include "ParamFader.h"
#include "VoiceOptions.h"
#include "NoiseGenerator.h"
#include "SineWavetable.h"
#include "SawWavetable.h"
#include "TriangleWavetable.h"
#include "SquareWavetable.h"
#include "TriangleOscillator.h"
#include "SquareOscillator.h"
#include "HarmonicClusterWavetable.h"
#include "FibonacciWavetable.h"
#include "MirrorWavetable.h"
#include "ModuloWavetable.h"
#include "BitWavetable.h"
#include "KorgonFilter.h"
#include "DSP.h"
#include "DSPObject.h"
#include "dsp_types.h"
#include "dsp_math.h"
#include "ADSR.h"
#include "clamp.h"
#include <cmath>
#include "DSPSampleBuffer.h"

struct ADSRParams
{
    dsp_float attackTime;
    dsp_float decayTime;
    dsp_float sustainLevel;
    dsp_float releaseTime;
    dsp_float attackShape;
    dsp_float releaseShape;
};

// The Voice class manages two oscillators and handles interactions like
// Frequency Modulation (FM) and Oscillator Sync between them.
class JPVoice : public DSPObject
{
public:
    // Constructor: requires two Oscillator pointers and the global sample rateSample
    JPVoice();

    // Destructor: deletes both oscillator instances
    ~JPVoice();

    // Initializes the DSP object
    void initialize() override;

    // Start ADSRs
    void playNote();

    // Stop  ADSRs
    void stopNote();

    // Sets the modulation index for frequency modulation.
    // This controls the intensity of the frequency modulation effect.
    void setModIndex(dsp_float index);

    // Enables or disables oscillator sync
    void setSyncEnabled(bool enabled);

    // Sets the pitch offset for the modulator
    void setPitchOffset(int offset);

    // Sets the fine tunig for the modulator
    void setFineTune(dsp_float fine);

    // Sets the frequency of oscillator 1/carrier
    void setFrequency(dsp_float f);

    // Sets the number of voices
    void setNumVoices(int count);

    // Sets the volume level of the oscillators
    void setOscillatorMix(dsp_float mix);

    // Sets the volume level of the noise generator
    void setNoiseMix(dsp_float mix);

    // Assigns the carrier oscillator
    void setCarrierOscillatorType(CarrierOscillatiorType oscillatorType);

    // Assigns the modulator oscillator
    void setModulatorOscillatorType(ModulatorOscillatorType oscillatorType);

    // Changes the current noise type (white or pink)
    void setNoiseType(NoiseType type);

    // Sets the detune factor
    void setDetune(dsp_float value);

    // Sets the feedback amount for the carrier
    void setFeedbackCarrier(dsp_float feedback);

    // Sets the feedback amount for the modulator
    void setFeedbackModulator(dsp_float feedback);

    // Sets the filter type
    void setFilterMode(FilterMode mode);

    // Sets the cutoff frequency
    void setFilterCutoff(dsp_float f);

    // Sets the filter resonance
    void setFilterResonance(dsp_float r);

    // Sets the filter drive
    void setFilterDrive(dsp_float value);

    // Sets the output buffers
    void setOutputBuffer(host_float *bufL, host_float *bufR);

    // Filter envelope params
    void setFilterADSR(ADSRParams &params);

    // Amplification envelope params
    void setAmpADSR(ADSRParams &params);

    // Link the ADSRs
    void linkADSR(bool isEnabled);

    // Set adrss to one shot mode
    void setAdsrOneshot(bool isEnabled);

    // Sets the amp envelope gain
    void setAmpGain(dsp_float g);

    // Next sample block generation
    void computeSamples();

private:
    WavetableOscillator *carrier;      // Carrier oscillator (may be modulated)
    WavetableOscillator *modulator;    // Modulator oscillator (for FM or sync)
    WavetableOscillator *carrierTmp;   // Carrier oscillator for oscillator change
    WavetableOscillator *modulatorTmp; // Modulator oscillator for oscillator change

    dsp_float frequency = 0.0; // Current frequency

    dsp_float modulationIndex = 0; // FM depth: how much modulator modulates carrier

    dsp_float oscmix = 0.0;   // Mix carrier <=> modulator
    dsp_float noisemix = 0.0; // Mix oscillators <=> noise
    bool syncEnabled = false; // True if Sync is active

    dsp_float detune = 0;       // Detune factor supersaw oszillator
    dsp_float pulseWidth = 0.5; // Pulse width square oscillator
    int pitchOffset = 0;        // Pitch offset modulator
    dsp_float fineTune = 0;     // Fine tune modulator

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

    // Multi mode filter
    KorgonFilter filter;
    // Buffer for filter resonance, cutoff is controlled by filterAdsr
    DSPSampleBuffer resoBuffer;

    // Modulation objects
    ADSR filterAdsr;
    ADSR ampAdsr;
    bool adsrLinked;
    void setFilterADSRLink(ADSRParams &params, bool setOther);
    void setAmpADSRLink(ADSRParams &params, bool setOther);


    // DSP working vars
    dsp_float carrierLeft, carrierRight;
    dsp_float modLeft, modRight;
    dsp_float mixL, mixR;
    dsp_float amp_carrier;
    dsp_float amp_modulator;
    dsp_float amp_oscmix;
    dsp_float amp_osc_noise;
    dsp_float amp_noise;

    // Output buffers of host
    DSPSampleBuffer mixBufferL; // Mixing buffer left channel
    DSPSampleBuffer mixBufferR; // Mixing buffer right channel

    // Feedback
    dsp_float lastSampleCarrierLeft;
    dsp_float lastSampleCarrierRight;
    dsp_float lastSampleModulatorLeft;
    dsp_float lastSampleModulatorRight;
    dsp_float feedbackAmountCarrier = 0.0;
    dsp_float feedbackAmountModulator = 0.0;

    // Number of voices
    int numVoices = 1;

    // Parameter change fader
    ParamFader paramFader;
};
