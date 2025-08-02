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

void SoundProcessor::connectProcessToBus(DSPAudioBus &bus)
{
    processBus = bus;
    onProcessBusConnected(bus);
}

void SoundProcessor::connectModulationToBus(DSPModulationBus &bus)
{
    modulationBus = bus;
    onModulationBusConnected(bus);
}

void SoundProcessor::initializeProcessor()
{
}

void SoundProcessor::initializeProcessor(size_t /*count*/)
{
}

void SoundProcessor::onProcessBusConnected(DSPAudioBus & /*bus*/)
{
}

void SoundProcessor::onModulationBusConnected(DSPModulationBus & /*bus*/)
{
}