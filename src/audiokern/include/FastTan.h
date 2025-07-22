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

protected:
    // Initializes the instance
    DSPUsage initializeObject() override;

private:
    // Processes data in bufferL, buffer R
    static void processBlock(DSPObject *dsp);
};