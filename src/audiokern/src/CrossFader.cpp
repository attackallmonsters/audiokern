#include "CrossFader.h"

CrossFader::CrossFader()
{
    registerBlockProcessor(&CrossFader::processBlock);
}

void CrossFader::initialize()
{
    slew.initialize();
    slew.setSlewTime(1.0);

    inputBufferAL.create(DSP::blockSize);
    inputBufferAR.create(DSP::blockSize);
    
    inputBufferBL.create(DSP::blockSize);
    inputBufferBR.create(DSP::blockSize);

    outputBufferL.create(DSP::blockSize);
    outputBufferR.create(DSP::blockSize);

    setMix(0.5);
}

void CrossFader::setMix(double value)
{
    // Clamp between 0.0 and 1.0
    mix = std::clamp(value, 0.0, 1.0);
    slew.setTarget(mix);
}

void CrossFader::processBlock(DSPObject *dsp)
{
    CrossFader *self = static_cast<CrossFader *>(dsp);

    dsp_float gainA, gainB;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        dsp_math::get_sin_cos(self->slew.process() * 0.5 * M_PI, &gainB, &gainA);

        self->outputBufferL[i] = self->inputBufferAL[i] * gainA + self->inputBufferBL[i] * gainB;
        self->outputBufferR[i] = self->inputBufferAR[i] * gainA + self->inputBufferBR[i] * gainB;
    }
}
