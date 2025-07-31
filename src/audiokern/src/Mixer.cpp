#include "Mixer.h"

Mixer::Mixer()
{
    registerBlockProcessor(&Mixer::processBlock);
}

void Mixer::initializeObject(size_t count)
{
    busCount = count;

    busNames.clear();
    busNames.resize(busCount);

    busses.clear();
    busses.resize(busCount);

    for (size_t i = 0; i < busCount; ++i)
    {
        busNames[i] = getName() + std::to_string(i);
        DSPBusManager::registerAudioBus(busNames[i]);
        busses[i] = DSPBusManager::getAudioBus(busNames[i]);
    }
}

void Mixer::connectOutputToBus(const std::string busName)
{
    outputBus = DSPBusManager::getAudioBus(busName);
}

const std::string Mixer::getInputBusName(size_t index)
{
    return busNames[index];
}

void Mixer::processBlock()
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        outputBus->l[i] = 0.0;
        outputBus->r[i] = 0.0;
    }

    for (size_t i = 0; i < busCount; ++i)
    {
        DSPAudioBus *inBus = busses[i];

        for (size_t l = 0; l < DSP::blockSize; ++l)
        {
            outputBus->l[l] += inBus->l[l];
            outputBus->r[l] += inBus->r[l];
        }
    }
}

void Mixer::processBlock(DSPObject *dsp)
{
    Mixer *self = static_cast<Mixer *>(dsp);
    self->processBlock();
}