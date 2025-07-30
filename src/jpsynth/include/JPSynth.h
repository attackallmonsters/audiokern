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
#include "AnalogDrift.h"

/**
 * @brief Lightweight structure representing a single active synth voice.
 */
struct SynthVoice
{
    JPVoice jpvoice; ///< Synth voice DSP instance
    int note;        ///< Associated MIDI note
};

/**
 * @brief Enum placeholder for defining LFO modulation targets (to be implemented).
 */
enum class LFOTarget
{
    None,   ///< No target, not active (LFO1, LFO2)
    Cutoff, ///< Filter cutof (LFO1)
    Amp,    ///< Amplification (LFO1)
};

/**
 * @brief Container for setting LFO modulation parameters.
 */
struct LFOParams
{
    LFOType type;         ///< LFO waveform type
    host_float frequency; ///< Frequency in Hz
    host_float offset;    ///< Offset added to LFO output
    host_float depth;     ///< Modulation depth
    host_float shape;     ///< Waveform shaping (if supported)
    host_float pw;        ///< Pulse width (if applicable)
    host_float smooth;    ///< Smoothing factor
    LFOTarget target;     ///< LFO target paramter
};

/**
 * @brief Central class representing a complete polyphonic synthesizer.
 *
 * JPSynth encapsulates all core components of a modular software synthesizer,
 * including oscillator configuration, modulation, envelopes, filtering, delay,
 * reverb, analog drift simulation and multithreaded voice rendering.
 *
 * The class is implemented as a singleton and provides a high-level API for real-time use.
 */
class JPSynth
{
public:
    /** @brief Returns the singleton instance. */
    static JPSynth &instance();

    /**
     * @brief Initializes the synthesizer.
     *
     * Sets up voice structures, routing and connects to output buses.
     * Must be called once after the DSP engine is initialized.
     *
     * @param outL Pointer to output buffer left
     * @param outR Pointer to output buffer right
     */
    void initialize(host_float *outL, host_float *outR);

    /** @brief Triggers a note-on event with given velocity. */
    void noteIn(int note, host_float velocity);

    /** @brief Sets the mix between carrier and modulator oscillators. */
    void setOscillatorMix(host_float mix);

    /** @brief Sets the noise oscillator volume mix. */
    void setNoiseMix(host_float mix);

    /** @brief Sets pitch offset (in semitones) for the modulator. */
    void setPitchOffset(host_float offset);

    /** @brief Sets fine tuning (in cents) for the modulator. */
    void setFineTune(host_float fine);

    /** @brief Sets detune amount for the carrier oscillator voices. */
    void setDetune(host_float detune);

    /** @brief Sets the carrier oscillator waveform type. */
    void setCarrierOscillatorType(CarrierOscillatiorType carrierType);

    /** @brief Sets the modulator oscillator waveform type. */
    void setModulatorOscillatorType(ModulatorOscillatorType modulatorType);

    /** @brief Selects the active noise type. */
    void setNoiseType(NoiseType noiseType);

    /** @brief Sets the modulation index for phase modulation. */
    void setModulation(host_float idx);

    /** @brief Sets pitch bend (as float multiplier). */
    void setPitchBend(host_float bend);

    /** @brief Sets the number of unison voices for the carrier oscillator. */
    void setNumVoices(int numVoices);

    /** @brief Enables or disables hard sync between modulator and carrier. */
    void setSyncEnabled(bool enable);

    /** @brief Sets the feedback amount applied to the carrier oscillator. */
    void setFeedbackCarrier(host_float fb);

    /** @brief Sets the feedback amount applied to the modulator oscillator. */
    void setFeedbackModulator(host_float fb);

    /** @brief Sets the base cutoff frequency of the filter. */
    void setFilterCutoff(host_float f);

    /** @brief Sets the resonance amount of the filter. */
    void setFilterResonance(host_float r);

    /** @brief Sets the nonlinear drive factor of the filter. */
    void setFilterDrive(host_float d);

    /** @brief Selects the filter mode (lowpass, highpass, etc). */
    void setFilterMode(FilterMode mode);

    /** @brief Enables or disabled cutoff follow */
    void setFilterFollow(bool enabled);

    /** @brief Sets the ADSR envelope parameters for the filter. */
    void setFilterADSR(ADSRParams adsr);

    /** @brief Sets the ADSR envelope parameters for the amplifier. */
    void setAmpADSR(ADSRParams adsr);

    /** @brief Enables or disables linking of filter and amp envelopes. */
    void linkADSR(bool enable);

    /** @brief Enables one-shot mode for all envelopes (ignores note off). */
    void setADSROneshot(bool enable);

    /** @brief Sets the LFO 1 parameters. */
    void setLFO1(LFOParams params);

    /** @brief Sets the LFO 2 parameters. */
    void setLFO2(LFOParams params);

    /** @brief Sets the size of the early reflection reverb stage. */
    void setReverbSpace(host_float space);

    /** @brief Sets the perceived room size of the reverb. */
    void setReverbRoom(host_float room);

    /** @brief Sets the damping filter cutoff frequency of the reverb. */
    void setReverbDamping(host_float damping);

    /** @brief Sets the density parameter of the reverb decay. */
    void setReverbDensity(host_float density);

    /** @brief Sets the stereo time ratio between early and late reverb. */
    void setReverbTimeRatio(dsp_math::TimeRatio ratio);

    /** @brief Sets the reverb wet level. */
    void setReverbWet(host_float vol);

    /** @brief Sets delay time per channel (L and R in seconds). */
    void setDelayTime(host_float timeL, host_float timeR);

    /** @brief Sets feedback gain per channel for the delay. */
    void setDelayFeedback(host_float fbL, host_float fbR);

    /** @brief Sets stereo time ratio between delay lines. */
    void setDelayTimeRatio(dsp_math::TimeRatio ratio);

    /** @brief Sets the delay wet level. */
    void setDelayWet(host_float vol);

    /** @brief Sets the total wetness of the combined effect chain. */
    void setWet(host_float wet);

    /** @brief Sets the analog feeling (amound and damping) */
    void setAnalogDrift(host_float amount, host_float damping);

    /** @brief Renders the full audio block from all voices and effect units. */
    void process();

private:
    void processVoiceBlock(); ///< Internal voice rendering
    void createVoices();      ///< Initializes voices

    SynthVoice *currentVoice;             ///< Active voice pointer
    VoiceAllocator<SynthVoice> allocator; ///< Voice manager
    DSPThreadPool voiceThreads;           ///< Thread pool for parallel voice processing

    Mixer voiceMixer;    ///< Dry voice mixdown
    CrossFader wetFader; ///< Dry/wet fader

    AnalogDrift analogDrift; ///< analog feeling

    LFO lfo1ar; ///< First LFO audio rate
    LFO lfo2ar; ///< Second LFO audio rate

    LFOTarget lfo1Target; ///< Target parameter LFO1
    LFOTarget lfo2Target; ///< Target parameter LFO2

    ButterworthFilter butterworth; ///< High-pass filter at 80 Hz
    NebularReverb reverb;          ///< Reverb effect unit
    Delay delay;                   ///< Stereo delay unit

    TuningSystem carrierTuning;      ///< Tuning for carrier oscillator
    TuningSystem modulatorTuning;    ///< Tuning for modulator oscillator
    TuningSystem filterCutoffTuning; ///< Tuning for filter cutoff
    MidiProcessor midi;              ///< Internal MIDI handler

    const size_t voiceCount = 6;         ///< Maximum voice count
    const std::string name = "_JPSynth"; ///< Synth name for routing

    DSPAudioBus *outputBus;       ///< Final output bus (host)
    DSPAudioBus *wetBus;          ///< Internal wet signal bus
    DSPAudioBus *voicesOutputBus; ///< Pre-effect voice sum

    DSPModulationBus *modFilterCutoffBus; ///< Cutoff bis for filter modulation by LFO 1
    DSPModulationBus *modAmpBus;          ///< Amplification modulation by LFO1

    DSPModulationBus *lfo1DummyBus; ///< Dummy bus for LFO1
    DSPModulationBus *lfo2DummyBus; ///< Dummy bus for LFO2

    bool filterFollowEnabled; /// Indicates if filter cutoff follows the note
    host_float currentCutoff; ///< To reset filter cutoff when follow is disabled

    const std::string outputBusName = "audio_host";   ///< signal bus name for output
    const std::string wetBusName = "wet";             ///< signal bus name wet
    const std::string voicesOutputBusName = "voices"; ///< bus name for voice output

    const std::string modFilterCutoffBusName = "modCutoffBus"; ///< bus name of filter cutoff modulation bus
    const std::string modAmpBusName = "modAmpBus";             ///< Amplification modulation bus name

    const std::string lfo1DummyBusName = "lfo1dummy"; ///< dummy bus for LFO1
    const std::string lfo2DummyBusName = "lfo2dummy"; ///< dummy bus for LFO2
};
