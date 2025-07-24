#include "Delay.h"

Delay::Delay()
{
    registerBlockProcessor(processBlock);
}

void Delay::initializeEffect()
{
    delayBuffer.initialize("delayBuffer" + getName());
    paramFader.initialize("paramFader" + getName());
}

void Delay::onOutputBusConnected()
{
    paramFader.connectToProcessBuffer(outputBus->busName);
}

// Sets the maximum delay time
void Delay::setMaxTime(host_float timeMS)
{
    host_float t = clampmin(timeMS, 0.0);

    delayBuffer.setMaxTime(t);
}

void Delay::setTime(host_float timeMSL, host_float timeMSR)
{
    host_float tL = clampmin(timeMSL, 0.0);
    host_float tR = clampmin(timeMSR, 0.0);

    paramFader.change(
        [=]()
        {
            delayBuffer.setTime(tL, tR);
        });
}

void Delay::setFeedback(host_float fbL, host_float fbR)
{
    feedbackL = clamp(fbL, 0.0, 1.0);
    feedbackR = clamp(fbR, 0.0, 1.0);
}

void Delay::processBlock()
{
    delayBuffer.push(inputBus->l, inputBus->r);

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Read output (delayed samples) of ring buffer
        host_float delayedL = delayBuffer.outputBufferL[i];
        host_float delayedR = delayBuffer.outputBufferR[i];

        // Write signal to ring buffers feedback buffer
        delayBuffer.feedbackBufferL[i] = delayedL * feedbackL;
        delayBuffer.feedbackBufferR[i] = delayedR * feedbackR;

        // Signal output
        outputBus->l[i] = delayedL;
        outputBus->r[i] = delayedR;
    }

    paramFader.process();
}

void Delay::processBlock(DSPObject *obj)
{
    Delay *self = static_cast<Delay *>(obj);

    self->processBlock();
}
