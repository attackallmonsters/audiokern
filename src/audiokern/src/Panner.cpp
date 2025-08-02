#include "Panner.h"

Panner::Panner()
{
    registerBlockProcessor(&Panner::processBlockGain);
}

void Panner::initializeProcessor()
{
    setMode(PanningMode::Gain);
    //setPanning(0.5);
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
    modulationBus.fill(pan);
}

void Panner::processBlockGain()
{
    host_float gainL, gainR;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        dsp_math::get_sin_cos(modulationBus.m[i] * 0.5 * dsp_math::DSP_PI, &gainL, &gainR);

        processBus.l[i] = processBus.l[i] * gainL;
        processBus.r[i] = processBus.r[i] * gainR;
    }
}

void Panner::processBlockBlend()
{
    host_float gainL, gainR;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        dsp_math::get_sin_cos(modulationBus.m[i] * 0.5 * dsp_math::DSP_PI, &gainL, &gainR);

        host_float inL = processBus.l[i];
        host_float inR = processBus.r[i];

        processBus.l[i] = inL * gainL + inR * (1.0f - gainR);
        processBus.r[i] = inR * gainR + inL * (1.0f - gainL);
    }
}

void Panner::processBlockBlendMono()
{
    host_float gainL, gainR;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        dsp_math::get_sin_cos(modulationBus.m[i] * 0.5 * dsp_math::DSP_PI, &gainL, &gainR);

        host_float inL = processBus.l[i];
        host_float inR = processBus.r[i];

        processBus.l[i] = inL * gainL + inR * gainL;
        processBus.r[i] = inR * gainR + inL * gainR;
    }
}

void Panner::processBlockGain(DSPObject *dsp)
{
    Panner *self = static_cast<Panner *>(dsp);
    self->processBlockGain();
}

void Panner::processBlockBlend(DSPObject *dsp)
{
    Panner *self = static_cast<Panner *>(dsp);
    self->processBlockBlend();
}

void Panner::processBlockBlendMono(DSPObject *dsp)
{
    Panner *self = static_cast<Panner *>(dsp);
    self->processBlockBlendMono();
}
