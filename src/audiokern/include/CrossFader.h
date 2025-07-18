#pragma once

#include "DSPObject.h"
#include "DSPSampleBuffer.h"
#include "SlewLimiter.h"
#include "dsp_math.h"
#include "clamp.h"
#include <cmath>

class CrossFader : public DSPObject
{
public:
    // Contructor
    CrossFader();

    // Initializes the insatnce
    void initialize() override;

    // Sets mix amount: 0.0 = only input A, 1.0 = only input B
    void setMix(double value);

    // Input buffers channel A
    DSPSampleBuffer inputBufferAL;
    DSPSampleBuffer inputBufferAR;

    // Input buffers channel B
    DSPSampleBuffer inputBufferBL;
    DSPSampleBuffer inputBufferBR;

    // Output buffers
    DSPSampleBuffer outputBufferL;
    DSPSampleBuffer outputBufferR;

private:
    // Block processing
    static void processBlock(DSPObject *dsp);

    // Audio artifact prevention
    SlewLimiter slew;

    double mix; // Raw mix value (0.0 to 1.0)
};
