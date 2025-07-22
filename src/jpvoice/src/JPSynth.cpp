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

    DSP::log("=====> Initializing JPSYNTH...");

    voiceThreads.initialize(cpu_count() / 2);
    carrierTuning.initialize();
    modulatorTuning.initialize();
    midi.initialize();
    voiceMixer.initialize(voiceCount);
    butterworth.initialize();
    reverb.initialize();
    delay.initialize();
    wetFader.initialize();

    createVoices();

    butterworth.processBufferL = voiceMixer.outputBufferL;
    butterworth.processBufferR = voiceMixer.outputBufferR;

    butterworth.setFilterMode(FilterMode::HP);
    butterworth.setCutoffFrequency(200.0);

    delay.inputBufferL = butterworth.processBufferL;
    delay.inputBufferR = butterworth.processBufferR;

    reverb.inputBufferL = delay.outputBufferL;
    reverb.inputBufferR = delay.outputBufferR;

    wetFader.inputBufferAL = voiceMixer.outputBufferL;
    wetFader.inputBufferAR = voiceMixer.outputBufferR;
    wetFader.inputBufferBL = reverb.outputBufferL;
    wetFader.inputBufferBR = reverb.outputBufferR;
    wetFader.outputBufferL = outL;
    wetFader.outputBufferR = outR;

    reverb.setSpace(0.1);
    reverb.setRoomSize(0.95);

    delay.setFeedback(0.0, 0.0);
    delay.setTime(0.0, 0.0);

    wetFader.setMix(1.0);

    for (size_t i = 0; i < voiceCount; ++i)
    {
        SynthVoice *voice = allocator.getVoice(i);

        voice->jpvoice.setOutputBuffer(voiceMixer.getInputBufferL(i), voiceMixer.getInputBufferR(i));
    }

    DSP::log("");
    DSP::log("> %s <", getRandomSynthQuote().c_str());
    DSP::log("");
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

void JPSynth::setLFO1(LFOParams /*params*/)
{
}

void JPSynth::setLFO2(LFOParams /*params*/)
{
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

void JPSynth::setReverbVolume(host_float vol)
{
    reverb.setVolume(vol);
}

void JPSynth::setWet(host_float wet)
{
    wetFader.setMix(wet);
}

void JPSynth::processBlock()
{
    DSP::nextBlock();

    processVoiceBlock();

    voiceMixer.generateBlock();

    butterworth.generateBlock();

    delay.generateBlock();

    reverb.generateBlock();

    wetFader.generateBlock();
}

void JPSynth::processVoiceBlock()
{
    for (auto *voice : allocator.getVoices())
    {
        voiceThreads.execute([voice]()
                             { voice->jpvoice.generateBlock(); });
    }

    voiceThreads.wait();
}

void JPSynth::createVoices()
{
    allocator.clear();

    for (size_t i = 0; i < voiceCount; ++i)
    {
        std::unique_ptr<SynthVoice> voice = std::make_unique<SynthVoice>();

        voice->jpvoice.initialize();

        // Transfer ownership to allocator
        allocator.add(std::move(voice));
    }
}
