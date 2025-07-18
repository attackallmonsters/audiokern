#include "NebularReverb.h"

NebularReverb::NebularReverb()
{
    registerBlockProcessor(&NebularReverb::processBlock);
}

void NebularReverb::initialize()
{
    inputBufferL.initialize(DSP::blockSize);
    inputBufferR.initialize(DSP::blockSize);

    outputBufferL.create(DSP::blockSize);
    outputBufferR.create(DSP::blockSize);

    for (int i = 0; i < maxDelays; ++i)
    {
        delays[i].initialize();
    }

    setDensity(4);
    setRoomSize(0.5);
    setDamping(5000.0);
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
    for (auto &delay : delays)
        delay.setDamping(damping);
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

void NebularReverb::push(const DSPSampleBuffer &bufL, const DSPSampleBuffer &bufR)
{
    for (int i = 0; i < density; ++i)
    {
        delays[i].delayBuffer.push(bufL, bufR);
    }
}

void NebularReverb::processBlock()
{
    // Feeds the reverb with samples
    push(inputBufferL, inputBufferR);

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
