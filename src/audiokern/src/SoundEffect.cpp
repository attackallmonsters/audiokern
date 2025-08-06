#include "SoundEffect.h"
#include "dsp_math.h"

SoundEffect::SoundEffect()
{
    registerBlockProcessor(&SoundEffect::processBlock);
}

void SoundEffect::registerEffectBlockProcessor(BlockProcessor f)
{
    effectProcessBlockFunc = f;
}

void SoundEffect::initializeObject()
{
    initializeObject(0);
}

void SoundEffect::initializeObject(size_t count)
{
    modulationBusA = DSPModulationBus::create(dsp_math::unique_string_id("modEffectA" + getName()), DSP::blockSize);
    modulationBusA.fill(1.0);

    modulationBusB = DSPModulationBus::create(dsp_math::unique_string_id("modEffectB" + getName()), DSP::blockSize);
    modulationBusB.fill(1.0);

    wetBus = DSPAudioBus::create(dsp_math::unique_string_id("wetEffect" + getName()), DSP::blockSize);
    modulationBusB.fill(1.0);

    wetFader.initialize("fader" + getName());

    if (count == 0)
        initializeEffect();
    else
        initializeEffect(count);

    wetFader.connectInputBToBus(wetBus);

    onModulationBusAConnected(modulationBusA);
    onModulationBusBConnected(modulationBusB);
    onWetBusConnected(wetBus);
}

void SoundEffect::setWet(host_float vol)
{
    wetFader.setMix(clamp(vol, 0.0, 1.0));
}

void SoundEffect::connectInputToBus(DSPAudioBus &bus)
{
    inputBus = bus;
    onInputBusConnected(inputBus);

    wetFader.connectInputAToBus(bus);
}

void SoundEffect::connectOutputToBus(DSPAudioBus &bus)
{
    outputBus = bus;
    onOutputBusConnected(outputBus);

    wetFader.connectOutputToBus(bus);
}

void SoundEffect::setOutputBus(DSPAudioBus &bus)
{
    outputBus = bus;
    onOutputBusConnected(outputBus);

    wetFader.connectOutputToBus(bus);
}

void SoundEffect::connectModulationToBusA(DSPModulationBus &bus)
{
    modulationBusA = bus;
    onModulationBusAConnected(modulationBusA);
}
void SoundEffect::connectModulationToBusB(DSPModulationBus &bus)
{
    modulationBusB = bus;
    onModulationBusBConnected(modulationBusA);
}

// TODO: remove this
void SoundEffect::initializeEffect()
{
}

void SoundEffect::initializeEffect(size_t /*count*/)
{
}

void SoundEffect::onInputBusConnected(DSPAudioBus & /*bus*/)
{
}

void SoundEffect::onOutputBusConnected(DSPAudioBus & /*bus*/)
{
}

void SoundEffect::onModulationBusAConnected(DSPModulationBus & /*bus*/)
{
}

void SoundEffect::onModulationBusBConnected(DSPModulationBus & /*bus*/)
{
}

void SoundEffect::onWetBusConnected(DSPAudioBus & /*bus*/)
{
}

void SoundEffect::processBlock(DSPObject *dsp)
{
    SoundEffect *self = static_cast<SoundEffect *>(dsp);

    self->effectProcessBlockFunc(dsp);

    self->wetFader.process();
}
