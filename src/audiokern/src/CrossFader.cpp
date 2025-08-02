#include "CrossFader.h"

CrossFader::CrossFader()
{
    registerBlockProcessor(&CrossFader::processBlock);
}

void CrossFader::initializeObject()
{
    slew.initialize("slew" + getName());
    slew.setSlewTime(1.0);
    setMix(0.0);
}

void CrossFader::connectInputAToBus(DSPAudioBus &bus)
{
    inputBusA = bus;
}

void CrossFader::connectInputBToBus(DSPAudioBus &bus)
{
    inputBusB = bus;
}

void CrossFader::connectOutputToBus(DSPAudioBus &bus)
{
    outputBus = bus;
}

void CrossFader::setMix(double value)
{
    // Clamp between 0.0 and 1.0
    mix = clamp(value, 0.0, 1.0);
    slew.setTarget(mix);
}

void CrossFader::processBlock()
{
    host_float gainA, gainB;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        dsp_math::get_sin_cos(slew.process() * 0.5 * dsp_math::DSP_PI, &gainA, &gainB);

        outputBus.l[i] = inputBusA.l[i] * gainA + inputBusB.l[i] * gainB;
        outputBus.r[i] = inputBusA.r[i] * gainA + inputBusB.r[i] * gainB;
    }
}

void CrossFader::processBlock(DSPObject *dsp)
{
    CrossFader *self = static_cast<CrossFader *>(dsp);
    self->processBlock();
}
