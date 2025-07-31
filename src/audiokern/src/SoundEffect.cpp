#include "SoundEffect.h"

void SoundEffect::initializeObject()
{
    initializeEffect();
}

void SoundEffect::initializeObject(size_t count)
{
    initializeEffect(count);
}

void SoundEffect::connectInputToBus(const std::string &busName)
{
    inputBus = DSPBusManager::getAudioBus(busName);
    onInputBusConnected();
}

void SoundEffect::connectOutputToBus(const std::string &busName)
{
    outputBus = DSPBusManager::getAudioBus(busName);
    onOutputBusConnected();
}

void SoundEffect::connectModulationToBus(const std::string &busName)
{
    modulationBus = DSPBusManager::getModulationBus(busName);
    onModulationBusConnected();
}

void SoundEffect::initializeEffect()
{
}

void SoundEffect::initializeEffect(size_t /*count*/)
{
}

void SoundEffect::onInputBusConnected()
{
}

void SoundEffect::onOutputBusConnected()
{
}

void SoundEffect::onModulationBusConnected()
{
}