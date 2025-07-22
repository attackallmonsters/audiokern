#include "Panner.h"

Panner::Panner()
{
    registerBlockProcessor(&Panner::processBlockGain);
}

DSPUsage Panner::initializeObject()
{
    slew.initialize();
    slew.setSlewTime(1.0);

    setPanning(0.5);

    return DSPUsage::Process;
}

void Panner::setMode(PanningMode mode)
{
    switch (mode)
    {
    case PanningMode::Gain:
        registerBlockProcessor(&Panner::processBlockGain);
        break;
    case PanningMode::Blend:
        registerBlockProcessor(&Panner::processBlockBlend);
        break;
    case PanningMode::BlendMono:
        registerBlockProcessor(&Panner::processBlockBlendMono);
        break;
    default:
        break;
    }
}

void Panner::setPanning(double value)
{
    // Clamp between 0.0 and 1.0
    pan = clamp(value, 0.0, 1.0);
    slew.setTarget(pan);
}

void Panner::processBlockGain(DSPObject *dsp)
{
    Panner *self = static_cast<Panner *>(dsp);

    dsp_float gainL, gainR;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        dsp_math::get_sin_cos(self->slew.process() * 0.5 * dsp_math::DSP_PI, &gainR, &gainL);

        self->processBufferL[i] = self->processBufferL[i] * gainL;
        self->processBufferR[i] = self->processBufferR[i] * gainR;
    }
}

void Panner::processBlockBlend(DSPObject *dsp)
{
    Panner *self = static_cast<Panner *>(dsp);

    dsp_float gainL, gainR;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        dsp_math::get_sin_cos(self->slew.process() * 0.5 * dsp_math::DSP_PI, &gainR, &gainL);

        host_float inL = self->processBufferL[i];
        host_float inR = self->processBufferR[i];

        self->processBufferL[i] = inL * gainL + inR * (1.0f - gainR);
        self->processBufferR[i] = inR * gainR + inL * (1.0f - gainL);
    }
}

void Panner::processBlockBlendMono(DSPObject *dsp)
{
    Panner *self = static_cast<Panner *>(dsp);

    dsp_float gainL, gainR;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        dsp_math::get_sin_cos(self->slew.process() * 0.5 * dsp_math::DSP_PI, &gainR, &gainL);

        host_float inL = self->processBufferL[i];
        host_float inR = self->processBufferR[i];

        self->processBufferL[i] = inL * gainL + inR * gainL;
        self->processBufferR[i] = inR * gainR + inL * gainR;
    }
}
