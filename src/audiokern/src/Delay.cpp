#include "Delay.h"

Delay::Delay()
{
    registerBlockProcessor(processBlock);
}

DSPUsage Delay::initializeObject()
{
    delayBuffer.initialize();
    paramFader.initialize();

    return DSPUsage::Effect;
}

void Delay::onBuffersCreated()
{
    paramFader.audioInputFrom(*this);
}

// Sets the maximum delay time
void Delay::setMaxTime(dsp_float timeMS)
{
    dsp_float t = clampmin(timeMS, 0.0);

    delayBuffer.setMaxTime(t);
}

void Delay::setTime(dsp_float timeMSL, dsp_float timeMSR)
{
    dsp_float tL = clampmin(timeMSL, 0.0);
    dsp_float tR = clampmin(timeMSR, 0.0);

    paramFader.change(
        [=]()
        {
            delayBuffer.setTime(tL, tR);
        });
}

void Delay::setFeedback(dsp_float fbL, host_float fbR)
{
    feedbackL = clamp(fbL, 0.0, 1.0);
    feedbackR = clamp(fbR, 0.0, 1.0);
}

void Delay::processBlock()
{
    delayBuffer.push(inputBufferL, inputBufferR);

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Read output (delayed samples) of ring buffer
        host_float delayedL = delayBuffer.outputBufferL[i];
        host_float delayedR = delayBuffer.outputBufferR[i];

        // Write signal to ring buffers feedback buffer
        delayBuffer.feedbackBufferL[i] = delayedL * feedbackL;
        delayBuffer.feedbackBufferR[i] = delayedR * feedbackR;

        // Signal output
        outputBufferL[i] = delayedL;
        outputBufferR[i] = delayedR;
    }

    paramFader.generateBlock();
}

void Delay::processBlock(DSPObject *obj)
{
    Delay *self = static_cast<Delay *>(obj);

    self->processBlock();
}
