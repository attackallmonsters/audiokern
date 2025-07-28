#include "Meter.h"
#include <cmath>

void Meter::meterBlock(DSPSampleBuffer *bufL, DSPSampleBuffer *bufR)
{
    dsp_float sumL = 0.0;
    dsp_float sumR = 0.0;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        sumL += (*bufL)[i] * (*bufL)[i];
        sumR += (*bufR)[i] * (*bufR)[i];
    }

    dsp_float rmsL = std::sqrt(sumL / DSP::blockSize);
    dsp_float rmsR = std::sqrt(sumL / DSP::blockSize);

    smoothedRMSL = smoothing * smoothedRMSL + (1.0 - smoothing) * rmsL;
    smoothedRMSR = smoothing * smoothedRMSR + (1.0 - smoothing) * rmsR;

    send(smoothedRMSL, smoothedRMSR);
}

void Meter::send(dsp_float left, dsp_float right)
{
    char msg[64];

    snprintf(msg, sizeof(msg), "{\"L\":%.3f,\"R\":%.3f}", left, right);

    UdpSender::send("level", msg);
}
