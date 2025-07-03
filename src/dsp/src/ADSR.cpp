#include "ADSR.h"

ADSR::ADSR()
{
    registerBlockProcessor(&ADSR::processBlock);
}

void ADSR::initialize()
{
    sampleRateMS = DSP::sampleRate / 1000.0;
    gain = 1.0;
    currentEnv = 0.0;
    oneShot = false;
    curveBuffer.resize(DSP::blockSize);

    setAttack(10);
    setDecay(100);
    setSustain(0.7);
    setRelease(750);
    setAttackShape(0.0);
    setReleaseShape(0.0);
    setGain(1);

    startupSamples = std::max(1, static_cast<int>(startupTimeMS * sampleRateMS));

    enterPhase(ADSRPhase::Idle);
}

dsp_float ADSR::shapeToExponent(dsp_float f)
{
    dsp_float shape = clamp(f, -1.0, 1.0);
    return (shape < 0.0) ? 1.0 + shape * 0.9 : 1.0 + shape * 9.0;
}

dsp_float ADSR::powerLerp(dsp_float start, dsp_float end, dsp_float p, dsp_float shape)
{
    if (shape == 1.0)
        return start + (end - start) * p;
    dsp_float curved = (end < start) ? 1.0 - std::pow(1.0 - p, shape) : std::pow(p, shape);
    return start + (end - start) * curved;
}

void ADSR::setAttack(dsp_float ms)
{
    attackTime = clamp(ms, 0.0, MAX_TIME);
    attackSamples = std::max(1, static_cast<int>(attackTime * sampleRateMS));
}

void ADSR::setDecay(dsp_float ms)
{
    decayTime = clamp(ms, 0.0, MAX_TIME);
    decaySamples = std::max(1, static_cast<int>(decayTime * sampleRateMS));
}

void ADSR::setSustain(dsp_float level)
{
    sustainLevel = clamp(level, 0.0, 1.0);
}

void ADSR::setRelease(dsp_float ms)
{
    dsp_float offset = startAtCurrentEnv ? 0.0 : startupTimeMS;
    releaseTime = clamp(ms - offset, 0.0, MAX_TIME);
    releaseSamples = std::max(1, static_cast<int>(releaseTime * sampleRateMS));
}

void ADSR::setAttackShape(dsp_float f)
{
    attackShape = shapeToExponent(f);
}

void ADSR::setReleaseShape(dsp_float f)
{
    releaseShape = shapeToExponent(f);
}

void ADSR::setGain(dsp_float g)
{
    gain = clampmin(g, 0.0);
}

void ADSR::setOneShot(bool b)
{
    oneShot = b;
}

void ADSR::setStartAtCurrent(bool start)
{
    startAtCurrentEnv = start;
}

void ADSR::triggerStart()
{
    phaseStartEnv = currentEnv;
    enterPhase(startAtCurrentEnv ? ADSRPhase::Attack : ADSRPhase::Startup);
}

void ADSR::triggerStop()
{
    if (oneShot || phase == ADSRPhase::Idle || phase == ADSRPhase::Release)
        return;

    phaseStartEnv = currentEnv;
    enterPhase(ADSRPhase::Release);
}

void ADSR::enterPhase(ADSRPhase newPhase)
{
    phase = newPhase;
    currentSample = 0;

    switch (newPhase)
    {
    case ADSRPhase::Startup:
        phaseFunc = &ADSR::phaseStartup;
        break;
    case ADSRPhase::Attack:
        phaseFunc = &ADSR::phaseAttack;
        break;
    case ADSRPhase::Decay:
        phaseFunc = &ADSR::phaseDecay;
        break;
    case ADSRPhase::Sustain:
        phaseFunc = &ADSR::phaseSustain;
        break;
    case ADSRPhase::Release:
        phaseFunc = &ADSR::phaseRelease;
        break;
    default:
        phaseFunc = &ADSR::phaseIdle;
        break;
    }
}

void ADSR::phaseIdle()
{
    currentEnv = 0.0;
}

void ADSR::phaseStartup()
{
    dsp_float p = static_cast<dsp_float>(currentSample) / startupSamples;
    currentEnv = powerLerp(phaseStartEnv, 0.0, p, 1.0);

    if (++currentSample >= startupSamples)
    {
        phaseStartEnv = 0.0;
        enterPhase(ADSRPhase::Attack);
    }
}

void ADSR::phaseAttack()
{
    dsp_float p = static_cast<dsp_float>(currentSample) / attackSamples;
    currentEnv = powerLerp(phaseStartEnv, 1.0, p, attackShape);

    if (++currentSample >= attackSamples)
        enterPhase(ADSRPhase::Decay);
}

void ADSR::phaseDecay()
{
    dsp_float p = static_cast<dsp_float>(currentSample) / decaySamples;
    currentEnv = (1.0 - p) * (1.0 - sustainLevel) + sustainLevel;

    if (++currentSample >= decaySamples)
    {
        if (oneShot)
        {
            phaseStartEnv = currentEnv;
            enterPhase(ADSRPhase::Release);
        }
        else
        {
            enterPhase(ADSRPhase::Sustain);
        }
    }
}

void ADSR::phaseSustain()
{
    currentEnv = sustainLevel;
}

void ADSR::phaseRelease()
{
    dsp_float p = static_cast<dsp_float>(currentSample) / releaseSamples;
    currentEnv = powerLerp(phaseStartEnv, 0.0, p, releaseShape);
    if (++currentSample >= releaseSamples)
        enterPhase(ADSRPhase::Idle);
}

// Next sample block generation
void ADSR::processBlock(DSPObject *dsp)
{
    ADSR *adsr = static_cast<ADSR *>(dsp);
    size_t blocksize = DSP::blockSize;

    for (size_t i = 0; i < blocksize; ++i)
    {
        (adsr->*adsr->phaseFunc)();
        adsr->curveBuffer[i] = adsr->currentEnv * adsr->gain;
    }
}

dsp_float* ADSR::getBuffer()
{
    return curveBuffer.data();
}