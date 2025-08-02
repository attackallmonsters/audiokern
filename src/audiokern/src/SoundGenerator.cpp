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

void SoundGenerator::connectFMToBus(DSPAudioBus &bus)
{
    fmBus = bus;
    onFMBusConnected(bus);
}

void SoundGenerator::connectOutputToBus(DSPAudioBus &bus)
{
    outputBus = bus;
    onOutputBusConnected(bus);
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

void SoundGenerator::onFMBusConnected(DSPAudioBus & /*bus*/)
{

}

void SoundGenerator::onOutputBusConnected(DSPAudioBus & /*bus*/)
{

}