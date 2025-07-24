#include "NebularReverb.h"

NebularReverb::NebularReverb()
{
    registerBlockProcessor(&NebularReverb::processBlock);
}

void NebularReverb::initializeEffect()
{
    wetBus = DSPBusManager::registerAudioChannel("wetBus" + getName());

    delayNames.clear();
    delayNames.resize(maxDelays);

    delays.clear();
    delays.resize(maxDelays);

    delayBusses.clear();
    delayBusses.resize(maxDelays);
    
    for (int i = 0; i < maxDelays; ++i)
    {
        delayNames[i] = "delay_" + std::to_string(i) + getName();

        DSPBusManager::registerAudioChannel(delayNames[i]);

        delayBusses[i] = DSPBusManager::getAudioBus(delayNames[i]);

        delays[i] = new CombDelay();
        delays[i]->initialize(delayNames[i]);

        delays[i]->connectToOutputBus(delayNames[i]);

        delays[i]->setMaxTime(1000.0);
    }

    fader.initialize("fader" + getName());

    setDensity(0.5);
    setSpace(0.3);
    setRoomSize(0.8);
    setDamping(5000.0);
    setWet(0.5);
}

void NebularReverb::onInputBusConnected()
{
    for (int i = 0; i < maxDelays; ++i)
    {
        delays[i]->connectToInputBus(inputBus->busName);
    }
}

void NebularReverb::onOutputBusConnected()
{
    fader.connectToInputBusForA(inputBus->busName);
    fader.connectToInputBusForB(wetBus->busName);
    fader.connectToOutputBus(outputBus->busName);
}

void NebularReverb::setDensity(host_float dense)
{
    int d = static_cast<int>(std::round(2 + clamp(dense, 0.0, 1.0) * (maxDelays - 2)));

    if (d != density)
    {
        density = d;
        updateDelays();
    }
}

void NebularReverb::setSpace(host_float size)
{
    delayTime = clamp(size, 0.04, 1.0) * 200;
    updateDelays();
}

void NebularReverb::setDamping(host_float damping)
{
    for (auto &delay : delays)
        delay->setDamping(damping);
}

void NebularReverb::setRoomSize(host_float size)
{
    for (auto &delay : delays)
        delay->setFeedback(size);
}

void NebularReverb::updateDelays()
{
    for (int i = 0; i < density; ++i)
    {
        // Spread factors across a range for density variation
        host_float factor = 0.8 + 0.4 * (i / static_cast<host_float>(density - 1));
        delays[i]->setTime(delayTime * factor);
    }
}

void NebularReverb::setWet(host_float vol)
{
    wet = clamp(vol, 0.0, 1.0) * 2.0;

    fader.setMix(wet);
}

void NebularReverb::processBlock()
{
    for (int i = 0; i < density; ++i)
    {
        delays[i]->push();
    }

    for (int i = 0; i < density; ++i)
    {
        delays[i]->process();
    }

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        host_float sumL = 0.0;
        host_float sumR = 0.0;

        for (int j = 0; j < density; ++j)
        {
            sumL += delayBusses[j]->l[i];
            sumR += delayBusses[j]->r[i];
        }

        wetBus->l[i] = sumL / density;
        wetBus->r[i] = sumR / density;
    }

    fader.process();
}

void NebularReverb::processBlock(DSPObject *dsp)
{
    NebularReverb *self = static_cast<NebularReverb *>(dsp);
    self->processBlock();
}
