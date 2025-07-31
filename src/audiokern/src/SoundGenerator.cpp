#include "SoundGenerator.h"

void SoundGenerator::initializeObject()
{
    setRole(GeneratorRole::Carrier);

    initializeGenerator();
}

void SoundGenerator::initializeObject(size_t count)
{
    setRole(GeneratorRole::Carrier);

    initializeGenerator(count);
}

void SoundGenerator::connectFMToBus(const std::string &busName)
{
    fmBus = DSPBusManager::getAudioBus(busName);
    onFMBusConnected();
}

void SoundGenerator::connectOutputToBus(const std::string &busName)
{
    outputBus = DSPBusManager::getAudioBus(busName);
    onOutputBusConnected();
}

void SoundGenerator::setRole(GeneratorRole role)
{
    generatorRole = role;
}

void SoundGenerator::initializeGenerator()
{
}

void SoundGenerator::initializeGenerator(size_t /*count*/)
{
}

void SoundGenerator::onFMBusConnected()
{

}

void SoundGenerator::onOutputBusConnected()
{

}