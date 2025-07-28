#include "CombDelay.h"

CombDelay::CombDelay()
{
    registerBlockProcessor(processBlock);
}

void CombDelay::initializeEffect()
{
    delayBuffer.initialize("delayBuffer" + getName());
    delayBuffer.setTime(0.0, 0.0);
    paramFader.initialize("paramFader" + getName());

    feedbackBufferL = delayBuffer.feedbackBufferL;
    feedbackBufferR = delayBuffer.feedbackBufferR;

    dampingStateL = 0.0;
    dampingStateR = 0.0;
}

void CombDelay::onOutputBusConnected()
{
    paramFader.connectToProcessBus(outputBus->busName);
}

void CombDelay::setMaxTime(host_float timeMS)
{
    host_float t = clampmin(timeMS, 0.0);

    delayBuffer.setMaxTime(t);
}

void CombDelay::setTime(host_float timeMS)
{
    host_float tL = currentTime = clampmin(timeMS, 0.0);
    host_float tR;

    if (timeRatio != dsp_math::TimeRatio::NONE)
    {
        tR = dsp_math::getTimeRatio(tL, timeRatio);
    }
    else
    {
        tR = tL + offsetTime;
    }

    paramFader.change(
        [=]()
        {
            delayBuffer.setTime(tL, tR);
            delayBuffer.clear();
        });
}

void CombDelay::setTimeOffset(host_float offset)
{
    offsetTime = clamp(offset, 0.0, 10.0);
}

void CombDelay::setTimeRatio(dsp_math::TimeRatio ratio)
{
    timeRatio = ratio;
    setTime(currentTime);
}

void CombDelay::setFeedback(host_float fb)
{
    feedback = clamp(fb, 0.0, 0.999);
}

void CombDelay::setDamping(host_float freqHz)
{
    host_float f = clamp(freqHz, 0.0, 20000.0);

    dampingCoeff = std::exp(-2.0 * dsp_math::DSP_PI * f / DSP::sampleRate);
}

void CombDelay::push()
{
    delayBuffer.push(inputBus->l, inputBus->r);
}

void CombDelay::processBlock()
{
    if (feedback == 0.0)
    {
        outputBus->l.copy(delayBuffer.outputBufferL);
        outputBus->r.copy(delayBuffer.outputBufferR);

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
        outputBus->l[i] = delayedL;
        outputBus->r[i] = delayedR;
    }

    paramFader.process();
}

void CombDelay::processBlock(DSPObject *obj)
{
    CombDelay *self = static_cast<CombDelay *>(obj);

    self->processBlock();
}
