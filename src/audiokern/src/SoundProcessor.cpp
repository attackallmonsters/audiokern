#include "SoundProcessor.h"
#include "omfg.h"

void SoundProcessor::initializeObject()
{
    initializeProcessor();
}

void SoundProcessor::initializeObject(size_t count)
{
    initializeProcessor(count);
}

void SoundProcessor::connectToProcessBus(const std::string &busName)
{
    processBus = DSPBusManager::getAudioBus(busName);
    onProcessBusConnected();
}

void SoundProcessor::connectToModulationBus(const std::string &busName)
{
    modulationBus = DSPBusManager::getModulationBus(busName);
    onModulationBusConnected();
}

void SoundProcessor::initializeProcessor()
{
}

void SoundProcessor::initializeProcessor(size_t /*count*/)
{
}

void SoundProcessor::onProcessBusConnected()
{
}

void SoundProcessor::onModulationBusConnected()
{
}