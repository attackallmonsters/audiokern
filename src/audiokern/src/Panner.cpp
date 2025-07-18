#include "Panner.h"

Panner::Panner()
{
    registerBlockProcessor(&Panner::processBlock);
}

void Panner::initialize()
{
    slew.initialize();
    slew.setSlewTime(1.0);

    inputBufferL.create(DSP::blockSize);
    inputBufferR.create(DSP::blockSize);

    outputBufferL.create(DSP::blockSize);
    outputBufferR.create(DSP::blockSize);

    setPanning(0.5);
}

void Panner::setPanning(double value)
{
    // Clamp between 0.0 and 1.0
    pan = std::clamp(value, 0.0, 1.0);
    slew.setTarget(pan);
}

void Panner::processBlock(DSPObject *dsp)
{
    Panner *self = static_cast<Panner *>(dsp);

    dsp_float gainL, gainR;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        dsp_math::get_sin_cos(self->slew.process() * 0.5 * M_PI, &gainR, &gainL);

        self->outputBufferL[i] = self->inputBufferL[i] * gainL;
        self->outputBufferR[i] = self->inputBufferR[i] * gainR;
    }
}
