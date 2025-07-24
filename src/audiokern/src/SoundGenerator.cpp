#include "SoundGenerator.h"

void initializeObject()
{
    outputBus = DSPBusManager::getAudioBus(StandardBus::Output);
    fmBus = DSPBusManager::getAudioBus(StandardBus::FM);

    initializeGenerator();
}

void initializeObject(size_t count)
{
    outputBus = DSPBusManager::getAudioBus(StandardBus::Output);
    fmBus = DSPBusManager::getAudioBus(StandardBus::FM);

    initializeGenerator(count);
}

void initializeGenerator()
{
}

void initializeGenerator(size_t count)
{
}