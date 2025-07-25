#pragma once

#include "DSPThreadPool.h"
#include "JPVoice.h"
#include "DSPSampleBuffer.h"
#include "Mixer.h"
#include "TuningSystem.h"
#include "VoiceAllocator.h"
#include "MidiProcessor.h"
#include "LFO.h"
#include "dsp_runtime.h"
#include "NebularReverb.h"
#include "ButterworthFilter.h"
#include "CrossFader.h"
#include "Delay.h"

struct SynthVoice
{
    JPVoice jpvoice;
    int note;
};

enum class LFOTargets
{

};

struct LFOParams
{
    LFOType type;
    host_float frequency;
    host_float offset;
    host_float depth;
    host_float shape;
    host_float pw;
    host_float smooth;
};

class JPSynth
{
public:
    // Static instance
    static JPSynth &instance();

    // Initializes the instance
    void initialize(host_float *outL, host_float *outR);

    // Handles a note in event
    void noteIn(int note, host_float velocity);

    // Sets the oscillator mix
    void setOscillatorMix(host_float mix);

    // Sets the noise mix
    void setNoiseMix(host_float mix);

    // Sets the modulator pitch offset in halftones
    void setPitchOffset(host_float offset);

    // Sets the modulator finetuning in cent
    void setFineTune(host_float fine);

    // Sets the carrier voices detuning
    void setDetune(host_float detune);

    // Sets the carrier oscillator type
    void setCarrierOscillatorType(CarrierOscillatiorType carrierType);

    // Sets the modulator oscillator type
    void setModulatorOscillatorType(ModulatorOscillatorType modulatorType);

    // Sets the noise type
    void setNoiseType(NoiseType noiseType);

    // Sets the modulation (carrier phase modulation)
    void setModulation(host_float idx);

    // Sets the pitch bend
    void setPitchBend(host_float bend);

    // Sets the carrier number of voices
    void setNumVoices(int numVoices);

    // Enables oscillator sync
    void setSyncEnabled(bool enable);

    // Sets the carriers feedback amount
    void setFeedbackCarrier(host_float fb);

    // Sets the modulators feedback amount
    void setFeedbackModulator(host_float fb);

    // Sets the filter cutoff
    void setFilterCutoff(host_float f);

    // Sets the filter resonance
    void setFilterResonance(host_float r);

    // Sets the filter drive
    void setFilterDrive(host_float d);

    // Sets the filter mode
    void setFilterMode(FilterMode mode);

    // Sets the filter envelope parameters
    void setFilterADSR(ADSRParams adsr);

    // Sets the amplification envelope parameters
    void setAmpADSR(ADSRParams adsr);

    // Enable link filter and amp ADSR
    void linkADSR(bool enable);

    // Sets ADSRs to one shot mode (ignores key release)
    void setADSROneshot(bool enable);

    // Sets the parameters for LFO 1
    void setLFO1(LFOParams params);

    // Sets the parameters for LFO 2
    void setLFO2(LFOParams params);

    // Sets the reverb space reflections
    void setReverbSpace(host_float space);

    // Sets the reverb room size
    void setReverbRoom(host_float room);

    // Sets the reverb damping cutoff frequency
    void setReverbDamping(host_float damping);

    // Sets the reverb density
    void setReverbDensity(host_float density);

    // Sets the time behaviour in the reverb
    void setReverbTimeRatio(dsp_math::TimeRatio ratio);

    // Sets the reverb wetness
    void setReverbWet(host_float vol);

    // Sets the delay time left/right
    void setDelayTime(host_float timeL, host_float timeR);

    // Sets the delay feedback amount left/right
    void setDelayFeedback(host_float timeL, host_float timeR);

    // Sets the delay time ration L/R
    void setDelayTimeRatio(dsp_math::TimeRatio ratio);

    // Sets the delay wetness
    void setDelayWet(host_float vol);

    // Sets the wetness amount of the effect chain
    void setWet(host_float wet);

    // Computes the samples of all voices
    void processBlock();

private:
    // Creates the samples vor the voice buffer
    void processVoiceBlock();

    // Creates the voices
    void createVoices();

    // The currenttly active voice
    SynthVoice *currentVoice;

    // Poly voice allocation management
    VoiceAllocator<SynthVoice> allocator;

    // Thread pool for voices
    DSPThreadPool voiceThreads;

    // Voice mixer
    Mixer voiceMixer;

    // Cross fade dry<>wet
    CrossFader wetFader;

    // LFO 1
    LFO lfo1;

    // LFO 2
    LFO lfo2;

    // Butterworth fixed high pass 80 Hz
    ButterworthFilter butterworth;

    // The reverb
    NebularReverb reverb;

    // The delay
    Delay delay;

    // Tuning system carrier
    TuningSystem carrierTuning;

    // Tuning system modulator
    TuningSystem modulatorTuning;

    // MIDI support
    MidiProcessor midi;

    // The number of voices
    const size_t voiceCount = 6;

    const std::string name = "_JPSynth";

    DSPAudioBus *outputBus;
    DSPAudioBus *wetBus;
    DSPAudioBus *voicesOutputBus;

    const std::string outputBusName = "audio_host";
    const std::string wetBusName = "wet";
    const std::string voicesOutputBusName = "voices";
};