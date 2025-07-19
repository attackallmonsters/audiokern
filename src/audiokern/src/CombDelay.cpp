#include "CombDelay.h"

CombDelay::CombDelay()
{
    registerBlockProcessor(processBlock);
}

void CombDelay::initialize()
{
    delayBuffer.setTime(0);

    outputBufferL.create(DSP::blockSize);
    outputBufferR.create(DSP::blockSize);

    writeIndex = 0;

    dampingStateL = 0.0;
    dampingStateR = 0.0;
}

void CombDelay::setTime(dsp_float timeMS)
{
    dsp_float t = clamp(timeMS, 0.0, 200.0);

    delayBuffer.setTime(t);
}

void CombDelay::setFeedback(dsp_float fb)
{
    feedback = std::clamp(fb, 0.0, 0.999);
}

void CombDelay::setDamping(dsp_float freqHz)
{
    dsp_float f = clamp(freqHz, 0.0, 20000.0);
        
    dampingCoeff = std::exp(-2.0 * M_PI * f / DSP::sampleRate);
}

void CombDelay::processBlock()
{
    // Ring buffer provides new samples in its output buffer
    delayBuffer.provideNextBlock();

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Read output (delayed samples) of ring buffer
        host_float delayedL = delayBuffer.outputBufferL[i];
        host_float delayedR = delayBuffer.outputBufferR[i];

        // Damping of feedback signal (y[n] = (1 - a) * x[n] + a * y[n - 1])
        dampingStateL = (1.0 - dampingCoeff) * delayedL + dampingCoeff * dampingStateL;
        dampingStateR = (1.0 - dampingCoeff) * delayedR + dampingCoeff * dampingStateR;

        // Write damped signal to ring buffers feedback buffer
        delayBuffer.feedbackBufferL[i] = delayedL + dampingStateL * feedback;
        delayBuffer.feedbackBufferR[i] = delayedR + dampingStateR * feedback;

        // Signal output
        outputBufferL[i] = delayedL;
        outputBufferR[i] = delayedR;
    }
}

void CombDelay::processBlock(DSPObject* obj)
{
    CombDelay* self = static_cast<CombDelay*>(obj);

    self->processBlock();
}
