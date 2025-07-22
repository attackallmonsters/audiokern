#include "CombDelay.h"

CombDelay::CombDelay()
{
    registerBlockProcessor(processBlock);
}

DSPUsage CombDelay::initializeObject()
{
    delayBuffer.initialize();
    delayBuffer.setTime(0.0, 0.0);
    paramFader.initialize();

    dampingStateL = 0.0;
    dampingStateR = 0.0;

    return DSPUsage::OutputOnly;
}

void CombDelay::onBuffersCreated()
{
    paramFader.audioInputFrom(*this);
}

void CombDelay::setMaxTime(dsp_float timeMS)
{
    dsp_float t = clampmin(timeMS, 0.0);

    delayBuffer.setMaxTime(t);
}

void CombDelay::setTime(dsp_float timeMS)
{
    dsp_float t = clampmin(timeMS, 0.0);

    paramFader.change(
        [=]()
        {
            delayBuffer.setTime(t, t);
            delayBuffer.clear();
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
    if (feedback == 0.0)
    {
        outputBufferL.copy(delayBuffer.outputBufferL);
        outputBufferR.copy(delayBuffer.outputBufferR);

        return;
    }

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
