#include "CombDelay.h"

CombDelay::CombDelay()
{
    registerBlockProcessor(processBlock);
}

void CombDelay::initialize()
{
    delayBuffer.initialize();
    delayBuffer.setTime(0);

    outputBufferL.create(DSP::blockSize);
    outputBufferR.create(DSP::blockSize);

    paramFader.outputBufferL = outputBufferL;
    paramFader.outputBufferR = outputBufferR;

    writeIndex = 0;

    dampingStateL = 0.0;
    dampingStateR = 0.0;
}

void CombDelay::setTime(dsp_float timeMS)
{
    dsp_float t = clampmin(timeMS, 0.0);

     paramFader.change(
        [=]()
        {
            delayBuffer.setTime(t);
        });
}

void CombDelay::setFeedback(dsp_float fb)
{
    feedback = clamp(fb, 0.0, 0.999);
}

void CombDelay::setDamping(dsp_float freqHz)
{
    dsp_float f = clamp(freqHz, 0.0, 20000.0);

    dampingCoeff = std::exp(-2.0 * dsp_math::DSP_PI * f / DSP::sampleRate);
}

void CombDelay::processBlock()
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Read output (delayed samples) of ring buffer
        host_float delayedL = delayBuffer.outputBufferL[i];
        host_float delayedR = delayBuffer.outputBufferR[i];

        // Damping of feedback signal (y[n] = (1 - a) * x[n] + a * y[n - 1])
        dampingStateL = (1.0 - dampingCoeff) * delayedL + dampingCoeff * dampingStateL;
        dampingStateR = (1.0 - dampingCoeff) * delayedR + dampingCoeff * dampingStateR;

        // Write damped signal to ring buffers feedback buffer
        delayBuffer.feedbackBufferL[i] = dampingStateL * feedback;
        delayBuffer.feedbackBufferR[i] = dampingStateR * feedback;

        // Signal output
        outputBufferL[i] = delayedL + delayBuffer.feedbackBufferL[i];
        outputBufferR[i] = delayedR + delayBuffer.feedbackBufferR[i];
    }

    paramFader.generateBlock();
}

void CombDelay::processBlock(DSPObject *obj)
{
    CombDelay *self = static_cast<CombDelay *>(obj);

    self->processBlock();
}
