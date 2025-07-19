#include "NebularReverb.h"

NebularReverb::NebularReverb()
{
    registerBlockProcessor(&NebularReverb::processBlock);

    delays.resize(maxDelays);
}

void NebularReverb::initialize()
{
    outputBufferL.create(DSP::blockSize);
    outputBufferR.create(DSP::blockSize);

    for (int i = 0; i < maxDelays; ++i)
    {
        delays[i].initialize();
    }

    setDensity(4);
    setRoomSize(0.5);
}

void NebularReverb::setDensity(int d)
{
    density = clamp(d, 2, maxDelays);
    updateDelays();
}

void NebularReverb::setSpace(dsp_float size)
{
    delayTime = clamp(size, 0.0, 1.0) * 100;
    updateDelays();
}

void NebularReverb::setDamping(dsp_float damping)
{
    dampingFrequency = clamp(damping, 0.0, 1.0) * 20000.0;

    for (auto &delay : delays)
        delay.setDamping(dampingFrequency);
}

void NebularReverb::setRoomSize(dsp_float size)
{
    for (auto &delay : delays)
        delay.setFeedback(size);
}

void NebularReverb::updateDelays()
{
    for (int i = 0; i < density; ++i)
    {
        // Spread factors across a range for density variation
        dsp_float factor = 0.8 + 0.4 * (i / static_cast<dsp_float>(density - 1));
        delays[i].setTime(delayTime * factor);
    }
}

// void NebularReverb::setBuffers(DSPSampleBuffer &bufL, DSPSampleBuffer &bufR)
// {
//     for (int i = 0; i < density; ++i)
//     {
//         delays[i].delayBuffer.push(bufL, bufR);
//     }
// }

void NebularReverb::processBlock()
{
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
            sumL += delays[j].outputBufferL[i];
            sumR += delays[j].outputBufferR[i];
        }

        outputBufferL[i] = sumL / density;
        outputBufferR[i] = sumR / density;
    }
}

void NebularReverb::processBlock(DSPObject *dsp)
{
    NebularReverb *self = static_cast<NebularReverb *>(dsp);

    self->processBlock();
}
