#include "LFO.h"

LFO::LFO()
{
    registerBlockProcessor(&LFO::processBlock);
}

void LFO::initialize()
{
    samplerate = DSP::sampleRate;
    lfoBuffer.resize(DSP::blockSize);

    phase = 0.0;
    phaseInc = 0.0;
    
    setFreq(0.0);
    setOffset(0.0);
    setDepth(1.0);
    setShape(0.0);
    setPulseWidth(0.5);
    setSmooth(0.0);
    setType(LFOType::Sine);
    setFreq(1.0);
}

inline dsp_float LFO::shapedRamp(dsp_float x)
{
    if (shape == 0.0)
        return x;
    else if (shape > 0.0)
        return std::pow(x, 1.0 + shape * 4.0); // convex
    else
        return 1.0 - std::pow(1.0 - x, 1.0 - shape * 4.0); // concave
}

void LFO::setSmooth(dsp_float f)
{
    f = clamp(f, 0.0, 1.0);
    smoothCoeff = 1.0 - f;
}

inline dsp_float LFO::lfoSine()
{
    return std::sin(phase * 2.0 * M_PI);
}

inline dsp_float LFO::lfoRampUp()
{
    return 2.0 * shapedRamp(phase) - 1.0;
}

inline dsp_float LFO::lfoRampDown()
{
    return 1.0 - 2.0 * shapedRamp(phase);
}

inline dsp_float LFO::lfoTriangle()
{
    dsp_float p = phase * 2.0;
    if (p < 1.0)
        return 2.0 * shapedRamp(p) - 1.0;
    else
        return 1.0 - 2.0 * shapedRamp(p - 1.0);
}

inline dsp_float LFO::lfoSquare()
{
    return (phase < pw) ? 1.0 : -1.0;
}

inline dsp_float LFO::lfoRandom()
{
    return 2.0 * ((dsp_float)rand() / RAND_MAX) - 1.0;
}

void LFO::setFreq(dsp_float f)
{
    freq = clampmin(f, 0.0);
    phaseInc = freq / samplerate;
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

void LFO::setOffset(dsp_float f)
{
    offset = f;
}

void LFO::setDepth(dsp_float f)
{
    depth = f;
}

void LFO::setShape(dsp_float f)
{
    shape = clamp(f, -1.0, 1.0);
}

void LFO::setPulseWidth(dsp_float f)
{
    pw = clamp(f, 0.01, 0.99);
}

void LFO::setIdleSignal(dsp_float f)
{
    idleSignal = f;
}

void LFO::reset()
{
    phase = 0.0;

    if (onPhaseWrap)
        onPhaseWrap();
}

void LFO::processBlock(DSPObject *dsp)
{
    LFO *lfo = static_cast<LFO *>(dsp);
    size_t n = DSP::blockSize;

    if (lfo->freq <= 0.0)
    {
        lfo->phase = 0.0;
        for (size_t i = 0; i < n; ++i)
            lfo->lfoBuffer[i] = lfo->idleSignal;
        return;
    }

    dsp_float phase = lfo->phase;
    dsp_float inc = lfo->phaseInc;

    for (size_t i = 0; i < n; ++i)
    {
        lfo->phase = phase;
        dsp_float val = (lfo->*lfo->lfoFunc)();

        dsp_float target = val * lfo->depth + lfo->offset;
        lfo->smoothVal += lfo->smoothCoeff * (target - lfo->smoothVal);
        lfo->lfoBuffer[i] = lfo->smoothVal;

        phase += inc;

        if (phase >= 1.0)
        {
            phase -= 1.0;

            if (lfo->onPhaseWrap)
                lfo->onPhaseWrap();
        }
    }

    lfo->phase = phase;
}

dsp_float *LFO::getBuffer()
{
    return lfoBuffer.data();
}