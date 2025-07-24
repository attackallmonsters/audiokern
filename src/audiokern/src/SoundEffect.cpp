#include "SoundEffect.h"

void SoundEffect::initializeObject()
{
    // Connect to standard audio buses
    inputBus = DSPBusManager::getAudioBus(StandardBusses::Input);
    wetBus   = DSPBusManager::getAudioBus(StandardBusses::Wet);

    // Connect to requested modulation bus
    modulationBus = DSPBusManager::getModulationBus(getName());

    modulationBusEnabled = modulationBus != nullptr;

    // Subclass-specific logic
    initializeEffect(pendingModulationName);
}

void SoundEffect::initializeObject(size_t count)
{
    // Connect to standard audio buses
    inputBus = DSPBusManager::getAudioBus(StandardBusses::Input);
    wetBus   = DSPBusManager::getAudioBus(StandardBusses::Wet);

    // Connect to requested modulation bus
    modulationBus = DSPBusManager::getModulationBus(pendingModulationName);
    if (!modulationBus)
    {
        DSPBusManager::registerModulationChannel(pendingModulationName);
        modulationBus = DSPBusManager::getModulationBus(pendingModulationName);
    }

    // Subclass-specific logic
    initializeEffect(pendingModulationName, count);
}

void SoundEffect::initializeEffect(const std::string& modulationBusName)
{
    // Default: no implementation
}

void SoundEffect::initializeEffect(const std::string& modulationBusName, size_t count)
{
    // Default fallback to single version
    initializeEffect(modulationBusName);
}
