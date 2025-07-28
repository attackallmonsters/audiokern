#include "Modulator.h"

void Modulator::initializeObject()
{
    // Subclass-specific logic
    initializeModulator();
}

void Modulator::initializeObject(size_t count)
{
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