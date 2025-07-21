#include "LFO.h"

LFO::LFO()
{
    registerBlockProcessor(&LFO::processBlockBuffer);
    initialize();
}

void LFO::initialize()
{
    samplerate = DSP::sampleRate;
    outputBuffer.initialize(DSP::blockSize);

    smoothVal = 0.0;
    phase = 0.0;
    phaseInc = 0.0;

    setMode(LFOMode::Buffered);
    setFreq(0.0);
    setOffset(0.0);
    setDepth(1.0);
    setShape(0.0);
    setPulseWidth(0.5);
    setSmooth(0.0);
    setType(LFOType::Sine);
    setFreq(1.0);
    isUnipolar(false);
}

inline host_float LFO::shapedRamp(host_float x)
{
    if (shape == 0.0)
        return x;
    else if (shape > 0.0)
        return std::pow(x, 1.0 + shape * 4.0); // convex
    else
        return 1.0 - std::pow(1.0 - x, 1.0 - shape * 4.0); // concave
}

void LFO::setSmooth(host_float f)
{
    f = clamp(f, 0.0, 1.0);
    smoothCoeff = 1.0 - f;
}

inline host_float LFO::lfoSine()
{
    return std::sin(phase * 2.0 * dsp_math::DSP_PI);
}

inline host_float LFO::lfoRampUp()
{
    return 2.0 * shapedRamp(phase) - 1.0;
}

inline host_float LFO::lfoRampDown()
{
    return 1.0 - 2.0 * shapedRamp(phase);
}

inline host_float LFO::lfoTriangle()
{
    host_float p = phase * 2.0;
    if (p < 1.0)
        return 2.0 * shapedRamp(p) - 1.0;
    else
        return 1.0 - 2.0 * shapedRamp(p - 1.0);
}

inline host_float LFO::lfoSquare()
{
    return (phase < pw) ? 1.0 : -1.0;
}

inline host_float LFO::lfoRandom()
{
    return 2.0 * ((host_float)rand() / RAND_MAX) - 1.0;
}

void LFO::setFreq(host_float f)
{
    freq = clampmin(f, 0.0);

    if (lfoMode == LFOMode::Value)
    {
        phaseInc = freq / samplerate * DSP::blockSize;
    }
    else
    {
        phaseInc = freq / samplerate;
    }
}

void LFO::setType(LFOType t)
{
    lfoType = t;

    switch (t)
    {
    case LFOType::Sine:
        lfoFunc = &LFO::lfoSine;
        break;
    case LFOType::RampUp:
        lfoFunc = &LFO::lfoRampUp;
        break;
    case LFOType::RampDown:
        lfoFunc = &LFO::lfoRampDown;
        break;
    case LFOType::Triangle:
        lfoFunc = &LFO::lfoTriangle;
        break;
    case LFOType::Square:
        lfoFunc = &LFO::lfoSquare;
        break;
    case LFOType::Random:
        lfoFunc = &LFO::lfoRandom;
        break;
    default:
        lfoFunc = &LFO::lfoSine;
        break;
    }
}

void LFO::setOffset(host_float f)
{
    offset = f;
}

void LFO::setDepth(host_float f)
{
    depth = f;
}

void LFO::setShape(host_float f)
{
    shape = clamp(f, -1.0, 1.0);
}

void LFO::setPulseWidth(host_float f)
{
    pw = clamp(f, 0.01, 0.99);
}

void LFO::setIdleSignal(host_float f)
{
    idleSignal = f;
}

void LFO::isUnipolar(bool enabled)
{
    unipolar = enabled;
}

void LFO::reset()
{
    phase = 0.0;

    if (onPhaseWrap)
        onPhaseWrap();
}

void LFO::setMode(LFOMode mode)
{
    if (mode == lfoMode)
    {
        return;
    }

    lfoMode = mode;

    setFreq(freq);

    setType(lfoType);

    switch (lfoMode)
    {
    case LFOMode::Buffered:
        registerBlockProcessor(&LFO::processBlockBuffer);
        break;

    case LFOMode::Value:
        registerBlockProcessor(&LFO::processBlockValue);
        break;
    }
}

void LFO::processBlockBuffer(DSPObject *dsp)
{
    LFO *lfo = static_cast<LFO *>(dsp);

    if (lfo->freq <= 0.0)
    {
        lfo->phase = 0.0;

        for (size_t i = 0; i < DSP::blockSize; ++i)
        {
            lfo->outputBuffer[i] = lfo->idleSignal;
        }

        return;
    }

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        host_float val = (lfo->*lfo->lfoFunc)();

        if (lfo->unipolar)
            val = 0.5 * (val + 1.0);

        lfo->smoothVal += lfo->smoothCoeff * (val - lfo->smoothVal);
        lfo->outputBuffer[i] = lfo->smoothVal * lfo->depth + lfo->offset;

        if (lfo->phase >= 1.0)
        {
            lfo->phase -= 1.0;

            if (lfo->onPhaseWrap)
                lfo->onPhaseWrap();
        }

        lfo->phase += lfo->phaseInc;
    }

    if (lfo->processLFOValue)
    {
        lfo->processLFOValue(lfo->outputBuffer[0]);
    }
}

void LFO::processBlockValue(DSPObject *dsp)
{
    LFO *lfo = static_cast<LFO *>(dsp);

    if (lfo->freq <= 0.0)
    {
        lfo->phase = 0.0;

        if (lfo->processLFOValue)
            lfo->processLFOValue(lfo->idleSignal);

        return;
    }

    host_float val = (lfo->*lfo->lfoFunc)();

    if (lfo->unipolar)
        val = 0.5 * (val + 1.0);

    if (lfo->processLFOValue)
        lfo->processLFOValue(val * lfo->depth + lfo->offset);

    if (lfo->phase >= 1.0)
    {
        lfo->phase -= 1.0;

        if (lfo->onPhaseWrap)
            lfo->onPhaseWrap();
    }

    lfo->phase += lfo->phaseInc;
}
