#pragma once

#include "DSPObject.h"
#include "DSPSampleBuffer.h"
#include "SlewLimiter.h"
#include "dsp_math.h"
#include "clamp.h"
#include <cmath>

class Panner : public DSPObject
{
public:
    // Contructor
    Panner();

    // Initializes the insatnce
    void initialize() override;

    // Sets panning amount: 0.0 = only left, 1.0 = only right
    void setPanning(double value);

    // Input buffers channel A
    DSPSampleBuffer inputBufferL;
    DSPSampleBuffer inputBufferR;

    // Output buffers
    DSPSampleBuffer outputBufferL;
    DSPSampleBuffer outputBufferR;

private:
    // Block processing
    static void processBlock(DSPObject *dsp);

    // Audio artifact prevention
    SlewLimiter slew;

    double pan; // Raw mix value (0.0 to 1.0)
};
