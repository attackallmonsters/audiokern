#include "Modulator.h"

void Modulator::initializeObject()
{
    const std::string& name = getName();

    // Register and connect a modulation bus using this object's name
    DSPBusManager::registerModulationChannel(name);
    modulationBus = DSPBusManager::getModulationBus(name);

    // Subclass-specific logic
    initializeModulator();
}

void Modulator::initializeObject(size_t count)
{
    const std::string& name = getName();

    // Register and connect a modulation bus using this object's name
    DSPBusManager::registerModulationChannel(name);
    modulationBus = DSPBusManager::getModulationBus(name);

    // Subclass-specific logic
    initializeModulator(count);
}

void Modulator::connectToModulationBus(const std::string &busName)
{
    modulationBus = DSPBusManager::getModulationBus(busName);
    onModulationBusConnected();
}

void Modulator::connectToProcessBuffer(const std::string &busName)
{
    processBus = DSPBusManager::getAudioBus(busName);
    onProcessBusConnected();
}

void Modulator::initializeModulator()
{
}

void Modulator::initializeModulator(size_t /*count*/)
{
}

void Modulator::onModulationBusConnected()
{
}

void Modulator::onProcessBusConnected()
{
}

void Modulator::multiply()
{
    process();
    
    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        processBus->l[i] *= modulationBus->m[i];
        processBus->r[i] *= modulationBus->m[i];
    }
}