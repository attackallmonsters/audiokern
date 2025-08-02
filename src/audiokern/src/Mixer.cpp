#include "Mixer.h"

Mixer::Mixer()
{
    registerBlockProcessor(&Mixer::processBlock);
}

void Mixer::initializeObject(size_t count)
{
    busCount = count;

    busses.clear();
    busses.resize(busCount);

    for (size_t i = 0; i < busCount; ++i)
    {
        busses[i] = &DSPAudioBus::create("mixbus_" + std::to_string(i) + getName(), DSP::blockSize);
    }
}

void Mixer::connectOutputToBus(DSPAudioBus &bus)
{
    outputBus = bus;
}

DSPAudioBus &Mixer::getInputBus(size_t index)
{
    return *busses[index];
}

void Mixer::processBlock()
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        outputBus.l[i] = 0.0;
        outputBus.r[i] = 0.0;
    }

    for (size_t i = 0; i < busCount; ++i)
    {
        DSPAudioBus *inBus = busses[i];

        for (size_t l = 0; l < DSP::blockSize; ++l)
        {
            outputBus.l[l] += inBus->l[l];
            outputBus.r[l] += inBus->r[l];
        }
    }
}

void Mixer::processBlock(DSPObject *dsp)
{
    Mixer *self = static_cast<Mixer *>(dsp);
    self->processBlock();
}