#include "TuningSystem.h"

void TuningSystem::initialize()
{
    setBaseFrequency(440.0);
    setIntervalSteps(12.0);
    setHalftoneOffset(0);
    setFinetune(0.0);
}

void TuningSystem::setBaseFrequency(dsp_float f)
{
    baseFrequency = clampmin(f, 0.0);
}

void TuningSystem::setIntervalSteps(dsp_float n)
{
    intervalSteps = clampmin(n, 1.0);
}

void TuningSystem::setHalftoneOffset(int halfTones)
{
    halftoneOffset = clamp(halfTones, -127, 127);
}

void TuningSystem::setFinetune(dsp_float ft)
{
    finetune = clamp(ft, -1200, 1200) / 1200.0;
}

dsp_float TuningSystem::frequency(int note)
{
    return baseFrequency * std::pow(2.0, (clampmin(note + halftoneOffset, 1) - 69) / intervalSteps + finetune);
}