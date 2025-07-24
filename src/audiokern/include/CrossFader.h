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

    // Audio input channel A from DSP object
    void audioInputForA(DSPObject &dspObject);

    // Audio input channel A from signal bus
    void audioInputForA(DSPSignalBus &signalBus);

    // Audio input channel A from buffers
    void audioInputForA(DSPSampleBuffer &bufferL, DSPSampleBuffer &bufferR);

    // Audio input channel B
    void audioInputForB(DSPObject &dspObject);

    // Audio input channel B from signal bus
    void audioInputForB(DSPSignalBus &signalBus);

    // Audio input channel B from buffers
    void audioInputForB(DSPSampleBuffer &bufferL, DSPSampleBuffer &bufferR);

    // Sets mix amount: 0.0 = only input A, 1.0 = only input B
    void setMix(double value);

    // Proceses the current block
    void processBlock();   

protected:
    // Initializes the insatnce
    DSPUsage initializeObject() override;

private:
    // Block processing
    static void processBlock(DSPObject *dsp);

    // Audio artifact prevention
    SlewLimiter slew;

    double mix; // Raw mix value (0.0 to 1.0)

    // Input buffers channel A
    DSPSampleBuffer inputBufferAL;
    DSPSampleBuffer inputBufferAR;

    // Input buffers channel B
    DSPSampleBuffer inputBufferBL;
    DSPSampleBuffer inputBufferBR;
};
