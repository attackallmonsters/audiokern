#pragma once

#include "DSPObject.h"
#include "DSPSampleBuffer.h"
#include "dsp_types.h"
#include "dsp_math.h"

class FastTan : public DSPObject
{
public:
    // Ctor
    FastTan();

    // Initializes the instance
    void initialize() override;

    // The samples to be filtered
    DSPSampleBuffer outputBufferL;
    DSPSampleBuffer outputBufferR;

private:
    // Processes data in bufferL, buffer R
    static void processBlock(DSPObject *dsp);
};