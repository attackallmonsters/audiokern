#include "Modulator.h"
#include "clamp.h"

void Modulator::initializeObject()
{
    gain = 1.0;

    // Subclass-specific logic
    initializeModulator();
}

void Modulator::initializeObject(size_t count)
{
    // Subclass-specific logic
    initializeModulator(count);
}

void Modulator::connectModulationToBus(DSPModulationBus &bus)
{
    modulationBus = bus;
    onModulationBusConnected(bus);
}

void Modulator::connectFMToBus(DSPModulationBus &bus)
{
    fmBus = bus;
    fmEnabled = true;
    onFMBusConnected(bus);
}

void Modulator::disconnectFMBus()
{
    fmEnabled = false;
    onFMBusDisconnected(fmBus);
}

void Modulator::setGain(host_float g)
{
    gain = clampmin(g, 0.0);
}

void Modulator::processMultiply(DSPAudioBus &targetBus)
{
    process();

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        targetBus.l[i] *= modulationBus.m[i];
        targetBus.r[i] *= modulationBus.m[i];
    }
}

void Modulator::processMultiply(DSPModulationBus &targetBus)
{
    process();

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        targetBus.m[i] *= modulationBus.m[i];
    }
}

void Modulator::initializeModulator()
{
}

void Modulator::initializeModulator(size_t /*count*/)
{
}

void Modulator::onModulationBusConnected(DSPModulationBus & /*bus*/)
{
}

void Modulator::onFMBusConnected(DSPModulationBus & /*bus*/)
{
}

void Modulator::onFMBusDisconnected(DSPModulationBus & /*bus*/)
{
}