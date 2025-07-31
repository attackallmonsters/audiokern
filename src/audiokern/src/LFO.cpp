#include "LFO.h"

LFO::LFO()
{
    registerBlockProcessor(&LFO::processBlockBuffer);
}

void LFO::initializeModulator()
{
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

    x = clamp(x, 0.0, 1.0);

    // convex : concave
    return (shape > 0.0) ? std::pow(x, 1.0 + shape * 4.0) : 1.0 - std::pow(1.0 - x, 1.0 - shape * 4.0);
}

void LFO::setSmooth(host_float f)
{
    f = clamp(f, 0.0, 0.8);
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
        phaseInc = clamp(freq / DSP::sampleRate * DSP::blockSize, 0.0, 1.0);
    }
    else
    {
        phaseInc = clamp(freq / DSP::sampleRate, 0.0, 1.0);
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

void LFO::processBlockBuffer()
{
    if (freq <= 0.0)
    {
        phase = 0.0;

        for (size_t i = 0; i < DSP::blockSize; ++i)
        {
            modulationBus->m[i] = idleSignal;
        }

        return;
    }

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        host_float val = (this->*lfoFunc)();

        if (unipolar)
            val = 0.5 * (val + 1.0);

        smoothVal += smoothCoeff * (val - smoothVal);
        modulationBus->m[i] = smoothVal * depth + offset;

        if (phase >= 1.0)
        {
            phase -= 1.0;

            if (onPhaseWrap)
                onPhaseWrap();
        }

        phase += phaseInc;
    }

    if (processLFOValue)
    {
        processLFOValue(modulationBus->m[0]);
    }
}

void LFO::processBlockValue()
{
    if (freq <= 0.0)
    {
        phase = 0.0;

        if (processLFOValue)
            processLFOValue(idleSignal);

        return;
    }

    host_float val = (this->*lfoFunc)();

    if (unipolar)
        val = 0.5 * (val + 1.0);

    if (processLFOValue)
        processLFOValue(val * depth + offset);

    if (phase >= 1.0)
    {
        phase -= 1.0;

        if (onPhaseWrap)
            onPhaseWrap();
    }

    phase += phaseInc;
}

void LFO::processBlockBuffer(DSPObject *dsp)
{
    LFO *self = static_cast<LFO *>(dsp);
    self->processBlockBuffer();
}

void LFO::processBlockValue(DSPObject *dsp)
{
    LFO *self = static_cast<LFO *>(dsp);
    self->processBlockValue();
}
