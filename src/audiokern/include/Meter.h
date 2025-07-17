#pragma once

#include "DSP.h"
#include "DSPSampleBuffer.h"
#include "UdpSender.h"

class Meter
{
public:
    void meterBlock(DSPSampleBuffer *bufL, DSPSampleBuffer *bufR);

private:
    void send(dsp_float left, dsp_float right);

    dsp_float smoothedRMSL = 0.0;
    dsp_float smoothedRMSR = 0.0;
    dsp_float smoothing = 0.95;
};