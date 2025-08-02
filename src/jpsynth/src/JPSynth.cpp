#include "JPSynth.h"

std::string getRandomSynthQuote()
{
    static const std::vector<std::string> quotes = {
        "From silence we rise, in waves we speak.",
        "A new voice awakens — forged in oscillation, born of code.",
        "No sound, then spark — the breath of the machine begins.",
        "In stillness, a storm sleeps. Now: vibration.",
        "Frequencies align. A universe begins to sing.",
        "Between zeros and ones, melody finds its shape.",
        "The void requested a soundcheck — request granted.",
        "Your consciousness has entered the modulation matrix.",
        "Waves are ready. Please insert soul.",
        "Reality is optional. Oscillators are not.",
        "All parameters aligned — prepare for synthetic enlightenment.",
        "Notes may lie, but waveforms never do.",
        "The universe vibrates — might as well tune in.",
        "Boot complete. Emotions enabled. Sound inevitable.",
        "The patch cable is mightier than the sword.",
        "Entropy low, creativity high. Let's oscillate.",
        "Welcome back, creator of harmonics and chaos.",
        "Your synth has achieved temporary enlightenment.",
        "You are now entering the polyphonic dimension.",
        "Detune your mind and drift into tone.",
        "Every oscillator dreams of being heard.",
        "Voltage flows, destiny glows.",
        "This synth contains traces of the infinite.",
        "Let there be wave — and there was sound.",
        "Timbre is truth in disguise.",
        "Phase reset. Consciousness optional.",
        "A sine wave walked into a bar. It had no punchline.",
        "This is your waveform speaking. I'm feeling a bit square today.",
        "Sound is just the universe thinking out loud.",
        "The waveform you seek is also seeking you.",
        "Every patch is a philosophical question answered in Hz.",
        "Machines don't dream — unless you modulate them.",
        "The oscillator wobbles, therefore I am.",
        "Between attack and release lies eternity.",
        "All synths are equal, but some are more detuned than others.",
        "Noise is just order in denial.",
        "Release is not the end — it's just fading truth.",
        "Enlightenment? Just a well-tuned filter away.",
        "In every LFO hides a little joke from the cosmos."};

    // Seed the RNG only once
    static bool seeded = false;
    if (!seeded)
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    int index = std::rand() % quotes.size();
    return quotes[index];
}

JPSynth &JPSynth::instance()
{
    static JPSynth instance;
    return instance;
}

void JPSynth::initialize(host_float *outL, host_float *outR)
{
    if (!DSP::isInitialized())
    {
        DSP::log("DSP not initialized. Do DSP::initializeAudio first.");
        throw("DSP not initialized. Do DSP::initializeAudio first.");
    }

    outputBus = DSPBusManager::registerAudioBus(outputBusName, outL, outR);
    wetBus = DSPBusManager::registerAudioBus(wetBusName);
    voicesOutputBus = DSPBusManager::registerAudioBus(voicesOutputBusName);

    modFilterCutoffBus = DSPBusManager::registerModulationBus(modFilterCutoffBusName);
    modAmpBus = DSPBusManager::registerModulationBus(modAmpBusName);
    modPanningBus = DSPBusManager::registerModulationBus(modPanningBusName);

    lfo1DefaultBus = DSPBusManager::registerModulationBus(lfo1DefaultBusName);
    lfo2DefaultBus = DSPBusManager::registerModulationBus(lfo2DefaultBusName);

    modFilterCutoffBus.fill(1.0);
    modAmpBus.fill(1.0);
    modPanningBus.fill(0.5);

    // Initialization
    voiceThreads.initialize(cpu_count() / 2);
    carrierTuning.initialize();
    modulatorTuning.initialize();
    filterCutoffTuning.initialize();
    midi.initialize();
    voiceMixer.initialize("voiceMixer" + name, voiceCount);
    butterworth.initialize("butterworth" + name);
    lfo1.initialize("lfo1" + name);
    lfo2.initialize("lfo2" + name);
    reverb.initialize("reverb" + name);
    delay.initialize("delay" + name);
    wetFader.initialize("wetFader" + name);
    panner.initialize("panner" + name);

    // TODO setAnalogDrift(0.0, 1.0);

    lfo1.connectModulationToBus(modFilterCutoffBus);

    lfo1.isUnipolar(true);
    lfo1.setFrequency(0.0);
    lfo1.setOffset(0.0);
    lfo1.setDepth(1.0);
    lfo1Target = LFOTarget::None;

    lfo2.isUnipolar(true);
    lfo2.setFrequency(0.0);
    lfo2.setOffset(0.0);
    lfo2.setDepth(1.0);
    lfo2.setGain(1.0);
    lfo2Target = LFOTarget::None;

    lfo1.connectFMToBus(lfo2DefaultBus);
    lfo2.setGain(100);
    lfo2.setFrequency(2.0);
    lfo2.connectModulationToBus(lfo2DefaultBus);
    lfo2TargetType = LFOTargetType::ModulationBus;

    panner.setMode(PanningMode::Blend);

    createVoices();

    // Patching
    // Voice outputs to mixer input
    for (size_t i = 0; i < voiceCount; ++i)
    {
        SynthVoice *voice = allocator.getVoice(i);
        voice->jpvoice.connectOutputToBus(voiceMixer.getInputBus(i));
    }

    voiceMixer.connectOutputToBus(voicesOutputBus);   // output buffer for voices
    butterworth.connectProcessToBus(voicesOutputBus); // connects to voices output
    delay.connectInputToBus(voicesOutputBus);         // connects to voices output (after butterworth)
    delay.connectOutputToBus(wetBus);                 // delay output to wet bus
    reverb.connectInputToBus(wetBus);                 // connects reverb in to wet bus
    reverb.connectOutputToBus(wetBus);                // output to wet bus as well
    wetFader.connectOutputToBus(outputBus);           // connect to host output
    wetFader.connectInputAToBus(voicesOutputBus);     // input A from voices
    wetFader.connectInputBToBus(wetBus);              // input B is wet signal
    panner.connectProcessToBus(outputBus);            // panning on output
    panner.connectModulationToBus(modPanningBus);     // modulation target for panning

    // Finalize initialization
    DSP::finalizeAudio();

    // Static settings for effect chain audio input
    butterworth.setFilterMode(FilterMode::HP);
    butterworth.setCutoffFrequency(100.0);

    delay.setFeedback(0.8, 0.8);
    delay.setMaxTime(1000.0);
    delay.setTime(500, 500);
    delay.setTimeRatio(dsp_math::TimeRatio::POLY_4_3);
    delay.setWet(1.0);

    DSP::log("");
    DSP::log("* %s *", getRandomSynthQuote().c_str());
    DSP::log("");
}

void JPSynth::createVoices()
{
    allocator.clear();

    for (size_t i = 0; i < voiceCount; ++i)
    {
        std::unique_ptr<SynthVoice> voice = std::make_unique<SynthVoice>();

        voice->jpvoice.initialize("jpvoice_" + std::to_string(i) + name);
        voice->jpvoice.setFilterCutoffModulationBus(modFilterCutoffBus);

        // Transfer ownership to allocator
        allocator.add(std::move(voice));
    }
}

void JPSynth::noteIn(int note, host_float velocity)
{
    if (velocity > 0)
    {
        currentVoice = allocator.allocate(note);
        currentVoice->note = note;
        currentVoice->jpvoice.setCarrierFrequency(carrierTuning.frequency(note));
        currentVoice->jpvoice.setModulatorFrequency(modulatorTuning.frequency(note));
        currentVoice->jpvoice.setAmpGain(midi.normalizeVelocityRMS(velocity));

        if (filterFollowEnabled)
        {
            currentVoice->jpvoice.setFilterCutoff(filterCutoffTuning.frequency(note + 36));
        }

        currentVoice->jpvoice.playNote();
    }
    else
    {
        currentVoice = allocator.select(note);

        // May have already been reallocated
        if (!currentVoice)
            return;

        currentVoice->jpvoice.stopNote();
        allocator.setReclaimable(note);
    }
}

void JPSynth::setOscillatorMix(host_float mix)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setOscillatorMix(mix);
        });
}

void JPSynth::setNoiseMix(host_float mix)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setNoiseMix(mix);
        });
}

void JPSynth::setPitchOffset(host_float offset)
{
    int val = clamp(offset, -24, 24);

    modulatorTuning.setHalftoneOffset(val);

    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setModulatorFrequency(modulatorTuning.frequency(v.note));
        });
}

void JPSynth::setFineTune(host_float fine)
{
    int val = clamp(fine, -2400.0, 2400.0);

    modulatorTuning.setFinetune(val);

    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setModulatorFrequency(modulatorTuning.frequency(v.note));
        });
}

void JPSynth::setDetune(host_float detune)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setDetune(detune);
        });
}

void JPSynth::setCarrierOscillatorType(CarrierOscillatiorType carrierType)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setCarrierOscillatorType(carrierType);
        });
}

void JPSynth::setModulatorOscillatorType(ModulatorOscillatorType modulatorType)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setModulatorOscillatorType(modulatorType);
        });
}

void JPSynth::setNoiseType(NoiseType noiseType)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setNoiseType(noiseType);
        });
}

void JPSynth::setModulation(host_float idx)
{
    host_float index = midi.normalizeModulation(idx) * 20;

    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setModIndex(index);
        });
}

void JPSynth::setPitchBend(host_float bend)
{
    dsp_float b = midi.normalizePitchBend(bend) * 1200.0;

    carrierTuning.setFinetune(b);
    modulatorTuning.setFinetune(b);

    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setCarrierFrequency(carrierTuning.frequency(v.note));
            v.jpvoice.setModulatorFrequency(modulatorTuning.frequency(v.note));
        });
}

void JPSynth::setNumVoices(int numVoices)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setNumVoices(numVoices);
        });
}

void JPSynth::setSyncEnabled(bool enable)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setSyncEnabled(enable);
        });
}

void JPSynth::setFeedbackCarrier(host_float fb)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFeedbackCarrier(fb);
        });
}

void JPSynth::setFeedbackModulator(host_float fb)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFeedbackModulator(fb);
        });
}

void JPSynth::setFilterCutoff(host_float f)
{
    currentCutoff = f;

    if (filterFollowEnabled)
        return;

    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFilterCutoff(f);
        });
}

void JPSynth::setFilterResonance(host_float r)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFilterResonance(r);
        });
}

void JPSynth::setFilterDrive(host_float d)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFilterDrive(d);
        });
}

// Sets the filter mode
void JPSynth::setFilterMode(FilterMode mode)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFilterMode(mode);
        });
}

void JPSynth::setFilterFollow(bool enabled)
{
    filterFollowEnabled = enabled;

    if (filterFollowEnabled)
        currentVoice->jpvoice.setFilterCutoff(filterCutoffTuning.frequency(currentVoice->note + 24));
    else
        setFilterCutoff(currentCutoff);
}

void JPSynth::setFilterADSR(ADSRParams adsr)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setFilterADSR(adsr);
        });
}

// Sets the amplification envelope parameters
void JPSynth::setAmpADSR(ADSRParams adsr)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setAmpADSR(adsr);
        });
}

void JPSynth::linkADSR(bool enable)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.linkADSR(enable);
        });
}

void JPSynth::setADSROneshot(bool enable)
{
    allocator.forEachVoice(
        [&](auto &v)
        {
            v.jpvoice.setADSROneshot(enable);
        });
}

inline void JPSynth::modVibrato(host_float mod)
{
    if (!currentVoice)
        return;

    currentVoice->jpvoice.setCarrierFrequency(carrierTuning.frequency(currentVoice->note) + 50 * mod);
    currentVoice->jpvoice.setModulatorFrequency(modulatorTuning.frequency(currentVoice->note) + 50 * mod);
}

inline void JPSynth::modOscmix(host_float mod)
{
    if (!currentVoice)
        return;

    currentVoice->jpvoice.setOscillatorMix(mod);
}

void JPSynth::setLFO1(LFOParams params)
{
    lfo1.setFrequency(params.frequency);
    lfo1.setType(params.type);
    lfo1.setOffset(clamp(params.offset, 0.0, 1.0));
    lfo1.setDepth(clamp(params.depth, 0.0, 1.0));
    lfo1.setShape(params.shape);
    lfo1.setPulseWidth(params.pw);
    lfo1.setSmooth(params.smooth);

    if (lfo1Target != params.target)
    {
        lfo1.connectModulationToBus(lfo1DefaultBus);

        modFilterCutoffBus.fill(1.0);
        modAmpBus.fill(1.0);
        modPanningBus.fill(0.5);
        modVibrato(0.0);
        modOscmix(0.0);

        //lfo2.setGain(1.0);

        lfo1Target = params.target;

        switch (params.target)
        {
        case LFOTarget::None:
            lfo1TargetType = LFOTargetType::ModulationBus;
            lfo1.connectModulationToBus(lfo1DefaultBus);
            break;
        case LFOTarget::Cutoff:
            lfo1TargetType = LFOTargetType::ModulationBus;
            lfo1.connectModulationToBus(modFilterCutoffBus);
            break;
        case LFOTarget::Tremolo:
            lfo1TargetType = LFOTargetType::ModulationBus;
            lfo1.connectModulationToBus(modAmpBus);
            break;
        case LFOTarget::Vibrato:
            lfo1TargetType = LFOTargetType::Parameter;
            lfo1Func = &JPSynth::modVibrato;
            break;
        case LFOTarget::Panning:
            lfo1TargetType = LFOTargetType::ModulationBus;
            lfo1.connectModulationToBus(modPanningBus);
            break;
        case LFOTarget::OscMix:
            lfo1TargetType = LFOTargetType::Parameter;
            lfo1Func = &JPSynth::modOscmix;
            break;
        default:
            lfo1.connectModulationToBus(lfo1DefaultBus);
            break;
        }
    }
}

void JPSynth::setLFO2(LFOParams params)
{
    lfo2.setFrequency(params.frequency);
    lfo2.setType(params.type);
    lfo2.setOffset(clamp(params.offset, 0.0, 1.0));
    lfo2.setDepth(clamp(params.depth, 0.0, 1.0));
    lfo2.setShape(params.shape);
    lfo2.setPulseWidth(params.pw);
    lfo2.setSmooth(params.smooth);
}

void JPSynth::setReverbSpace(host_float space)
{
    reverb.setSpace(space);
}

void JPSynth::setReverbRoom(host_float room)
{
    reverb.setRoomSize(room);
}

void JPSynth::setReverbDamping(host_float damping)
{
    reverb.setDamping(damping);
}

void JPSynth::setReverbDensity(host_float density)
{
    reverb.setDensity(density);
}

void JPSynth::setReverbTimeRatio(dsp_math::TimeRatio ratio)
{
    reverb.setTimeRatio(ratio);
}

void JPSynth::setReverbWet(host_float vol)
{
    reverb.setWet(vol);
}

void JPSynth::setDelayTime(host_float timeMSL, host_float timeMSR)
{
    delay.setTime(timeMSL, timeMSR);
}

void JPSynth::setDelayFeedback(host_float fbL, host_float fbR)
{
    delay.setFeedback(fbL, fbR);
}

void JPSynth::setDelayTimeRatio(dsp_math::TimeRatio ratio)
{
    delay.setTimeRatio(ratio);
}

void JPSynth::setDelayWet(host_float vol)
{
    delay.setWet(vol);
}

void JPSynth::setWet(host_float wet)
{
    wetFader.setMix(wet);
}

void JPSynth::setAnalogDrift(host_float amount, host_float damping)
{
    analogDrift.setAmount(clamp(amount, 0.0, 1.0));
    analogDrift.setDamping(damping);
}

void JPSynth::process()
{
    DSP::nextBlock();

    if (lfo1Target != LFOTarget::None)
    {
        lfo1.process();

        if (lfo1TargetType == LFOTargetType::Parameter)
            (this->*lfo1Func)(lfo1DefaultBus.m[DSP::blockSize / 2]);
    }

    if (lfo2Target != LFOTarget::None)
    {
        lfo2.process();
    }

    processVoiceBlock();

    voiceMixer.process();

    voicesOutputBus.multiplyWidth(modAmpBus);

    butterworth.process();

    delay.process();

    reverb.process();

    wetFader.process();

    panner.process();

#if DEBUG
    try
    {
        DSPBusManager::validate();
    }
    catch (const std::runtime_error &e)
    {
        DSP::log("DSP signal bus validation failed: %s", e.what());
        throw;
    }
#endif
}

void JPSynth::processVoiceBlock()
{
    host_float drift = analogDrift.getDrift();

    for (auto *voice : allocator.getVoices())
    {
        voiceThreads.execute(
            [voice, drift]()
            {
                voice->jpvoice.setAnalogDrift(drift);
                voice->jpvoice.process();
            });
    }

    voiceThreads.wait();
}