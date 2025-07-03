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
#include "clamp.h"
#include <cmath>


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
    void setFilterCutoff(DSPBuffer *buffer);

    // Sets the filter resonance
    void setFilterResonance(DSPBuffer *buffer);

    // Sets the filter drive
    void setFilterDrive(dsp_float value);

    // Next sample block generation
    void computeSamples();

    DSPBuffer mixBufferL; // Mixing buffer left channel
    DSPBuffer mixBufferR; // Mixing buffer right channel

private:
    WavetableOscillator *carrier;      // Carrier oscillator (may be modulated)
    WavetableOscillator *modulator;    // Modulator oscillator (for FM or sync)
    WavetableOscillator *carrierTmp;   // Carrier oscillator (may be modulated)
    WavetableOscillator *modulatorTmp; // Modulator oscillator (for FM or sync)

    dsp_float frequency = 0.0; // Current frequency

    dsp_float modulationIndex = 0;       // FM depth: how much modulator modulates carrier

    dsp_float oscmix = 0.0;   // Mix carrier <=> modulator
    dsp_float noisemix = 0.0; // Mix oscillators <=> noise
    bool syncEnabled = false; // True if Sync is active

    dsp_float detune = 0;       // Detune factor supersaw oszillator
    dsp_float pulseWidth = 0.5; // Pulse width square oscillator
    int pitchOffset = 0;        // Pitch offset modulator
    dsp_float fineTune = 0;     // Fine tune modulator

    // Oscillators
    NoiseGenerator *noise = new NoiseGenerator(); // Noise generator
    SineWavetable *sineCarrier = new SineWavetable();
    SineWavetable *sineModulator = new SineWavetable();
    SawWavetable *sawCarrier = new SawWavetable();
    SawWavetable *sawModulator = new SawWavetable();
    SquareWavetable *squareCarrier = new SquareWavetable();
    SquareWavetable *squareModulator = new SquareWavetable();
    TriangleWavetable *trianlgeCarrier = new TriangleWavetable();
    TriangleWavetable *triangleModulator = new TriangleWavetable();
    HarmonicClusterWavetable *clusterCarrier = new HarmonicClusterWavetable();
    HarmonicClusterWavetable *clusterModulator = new HarmonicClusterWavetable();
    FibonacciWavetable *fibonacciCarrier = new FibonacciWavetable();
    FibonacciWavetable *fibonacciModulator = new FibonacciWavetable();
    MirrorWavetable *mirrorCarrier = new MirrorWavetable();
    MirrorWavetable *mirrorModulator = new MirrorWavetable();
    ModuloWavetable *moduloCarrier = new ModuloWavetable();
    ModuloWavetable *moduloModulator = new ModuloWavetable();
    BitWavetable *bitModulator = new BitWavetable();

    // Multi mode filter
    KorgonFilter *filter = new KorgonFilter();

    // DSP working vars
    dsp_float carrierLeft, carrierRight;
    dsp_float modLeft, modRight;
    dsp_float mixL, mixR;
    dsp_float amp_carrier;
    dsp_float amp_modulator;
    dsp_float amp_oscmix;
    dsp_float amp_osc_noise;
    dsp_float amp_noise;

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
