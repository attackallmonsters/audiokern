#pragma once

#include "dsp_types.h"
#include "clamp.h"
#include <cmath>

class TuningSystem
{
public:
    // Initialization
    void initialize();

    // Sets the base frequency for an intervall
    void setBaseFrequency(dsp_float f);

    // Sets the steps for an interval
    void setIntervalSteps(dsp_float n);

    // Sets an offset in half tones
    void setHalftoneOffset(int halfTones);

    // Sets the fine tuning in cent
    void setFinetune(dsp_float ft);

    // Calculates the frequency of the given interval
    dsp_float frequency(int note);

private:
    dsp_float baseFrequency;
    dsp_float intervalSteps;
    dsp_float halftoneOffset;
    dsp_float finetune;
};