#pragma once

#include "JPVoice.h"
#include "DSPSampleBuffer.h"
#include "Mixer.h"
#include "TuningSystem.h"
#include "VoiceAllocator.h"
#include "MidiProcessor.h"

struct SynthVoice
{
    JPVoice jpvoice;
    int note;
};

class JPSynth
{
public:
    // Static instance
    static JPSynth &instance();

    // Initializes the instance
    void initialize(host_float *bufL, host_float *bufR);

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

    // Sets the filter mode
    void setFilterMode(FilterMode mode);

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

    // Sets the filter envelope parameters
    void setFilterADSR(ADSRParams adsr);

    // Sets the amplification envelope parameters
    void setAmpADSR(ADSRParams adsr);

    // Enable link filter and amp ADSR
    void linkADSR(bool enable);

    // Sets ADSRs to one shot mode (ignores key release)
    void setADSROneshot(bool enable);

    // Computes the samples of all voices
    void processBlock();

private:
    // Creates the voices
    void createVoices();

    // The currenttly active voice
    SynthVoice *currentVoice;

    // Poly voice allocation management
    VoiceAllocator<SynthVoice> allocator;

    // Mixer left channel
    Mixer mixerL;

    // Mixer right channel
    Mixer mixerR;

    // Tuning system carrier
    TuningSystem carrierTuning;

    // Tuning system modulator
    TuningSystem modulatorTuning;

    // MIDI support
    MidiProcessor midi;

    // The number of voices
    const size_t voiceCount = 1;
};