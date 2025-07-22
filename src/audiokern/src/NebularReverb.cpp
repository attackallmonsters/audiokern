#include "NebularReverb.h"

NebularReverb::NebularReverb()
{
    registerBlockProcessor(&NebularReverb::processBlock);
}

DSPUsage NebularReverb::initializeObject()
{
    for (int i = 0; i < maxDelays; ++i)
    {
        delays[i].initialize();
        delays[i].setMaxTime(1000.0);
        DSPBus bus = delays[i].getOutputBus();
        delayBusses.push_back(bus);
    }   

    setDensity(0.5);
    setSpace(0.0);
    setRoomSize(0.5);
    setDamping(5000.0);
    setWet(1.0);

    return DSPUsage::Effect;
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
        delay.setDamping(damping);
}

void NebularReverb::setRoomSize(host_float size)
{
    for (auto &delay : delays)
        delay.setFeedback(size);
}

void NebularReverb::updateDelays()
{
    for (int i = 0; i < density; ++i)
    {
        // Spread factors across a range for density variation
        host_float factor = 0.8 + 0.4 * (i / static_cast<host_float>(density - 1));
        delays[i].setTime(delayTime * factor);
    }
}

void NebularReverb::setWet(host_float vol)
{
    wet = clamp(vol, 0.0, 1.0) * 2.0;
}

void NebularReverb::push()
{
    for (int i = 0; i < density; ++i)
    {
        delays[i].delayBuffer.push(inputBufferL, inputBufferR);
    }
}

void NebularReverb::processBlock()
{
    push();

    for (int i = 0; i < density; ++i)
    {
        delays[i].processBlock();
    }

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        host_float sumL = 0.0;
        host_float sumR = 0.0;

        for (int j = 0; j < density; ++j)
        {
            sumL += (*delayBusses[j].left)[i];
            sumR += (*delayBusses[j].right)[i];
        }

        outputBufferL[i] = sumL / density * wet;
        outputBufferR[i] = sumR / density * wet;
    }
}

void NebularReverb::processBlock(DSPObject *dsp)
{
    NebularReverb *self = static_cast<NebularReverb *>(dsp);

    self->processBlock();
}
