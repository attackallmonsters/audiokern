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

void SoundProcessor::connectToProcessBuffer(const std::string &busName)
{
    processBus = DSPBusManager::getAudioBus(busName);
}

void SoundProcessor::connectToModulationBuffer(const std::string &busName)
{
    modulationBus = DSPBusManager::getModulationBus(busName);
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