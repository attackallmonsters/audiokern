#include "SoundEffect.h"

void SoundEffect::initializeObject()
{
    initializeEffect();
}

void SoundEffect::initializeObject(size_t count)
{
    initializeEffect(count);
}

void SoundEffect::connectInputToBus(DSPAudioBus &bus)
{
    inputBus = bus;
    onInputBusConnected(inputBus);
}

void SoundEffect::connectOutputToBus(DSPAudioBus &bus)
{
    outputBus = bus;
    onOutputBusConnected(outputBus);
}

void SoundEffect::setOutputBus(DSPAudioBus &bus)
{
    outputBus = bus;
    onOutputBusConnected(outputBus);
}

void SoundEffect::connectModulationToBus(DSPModulationBus &bus)
{
    modulationBus = bus;
    onModulationBusConnected(modulationBus);
}

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

void SoundEffect::onModulationBusConnected(DSPModulationBus & /*bus*/)
{
}