#include "NebularReverb.h"


NebularReverb::NebularReverb()
{
    registerBlockProcessor(&NebularReverb::processBlock);
}

void NebularReverb::initialize()
{
    

    for (int i = 0; i < density; ++i)
    {
        delaysL[i].initialize();
        delaysR[i].initialize();
    }
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

    for (auto& delay : delaysL)
        delay.setDamping(dampingFrequency);
}

void NebularReverb::setSpaceSize(dsp_float size)
{
    for (auto& delay : delaysL)
        delay.setFeedback(size);
}

void NebularReverb::updateDelays()
{
    for (int i = 0; i < density; ++i)
    {
        // Spread factors across a range for density variation
        dsp_float factor = 0.8 + 0.4 * (i / static_cast<dsp_float>(density - 1));
        delaysL[i].setTime(delayTime * factor);
    }
}

void NebularReverb::processBlock(DSPObject* dsp)
{
    NebularReverb* self = static_cast<NebularReverb*>(dsp);

    // for (size_t i = 0; i < DSP::blockSize; ++i)
    // {
    //     dsp_float accL = 0.0, accR = 0.0;

    //     for (int j = 0; j < self->density; ++j)
    //     {
    //         accL += self->delaysL[j].processSample(inL[i], 0); 
    //         accR += self->delaysR[j].processSample(inR[i], 1);
    //     }

    //     outL[i] = accL / self->density;
    //     outR[i] = accR / self->density;
    // }
}
