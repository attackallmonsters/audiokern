#include "Delay.h"

Delay::Delay()
{
    registerBlockProcessor(processBlock);
}

void Delay::initializeEffect()
{
    delayBuffer.initialize("delayBuffer" + getName());
    paramFader.initialize("paramFader" + getName());
    wetFader.initialize("wetFader");
    wetBus = DSPBusManager::registerAudioBus("wetBus" + getName());
}

void Delay::onOutputBusConnected()
{
    paramFader.connectToProcessBus(outputBus->busName);
    wetFader.connectToInputBusForA(inputBus->busName);
    wetFader.connectToInputBusForB(wetBus->busName);
    wetFader.connectToOutputBus(outputBus->busName);
}

// Sets the maximum delay time
void Delay::setMaxTime(host_float timeMS)
{
    host_float t = clampmin(timeMS, 0.0);

    delayBuffer.setMaxTime(t);
}

void Delay::setTime(host_float timeMSL, host_float timeMSR)
{
    host_float tL = timeMSL;
    host_float tR;

    if (timeRatio != dsp_math::TimeRatio::NONE)
    {
        tR = dsp_math::getTimeRatio(tL, timeRatio);
    }
    else
    {
        tR = timeMSR;
    }

    currentTimeL = tL;
    currentTimeR = tR;

    paramFader.change(
        [=]()
        {
            delayBuffer.setTime(tL, tR);
        });
}

void Delay::setTimeRatio(dsp_math::TimeRatio ratio)
{
    timeRatio = ratio;
    setTime(currentTimeL, currentTimeR);
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
        wetBus->l[i] = delayedL;
        wetBus->r[i] = delayedR;
    }

    wetFader.process();
    paramFader.process();
}

void Delay::setWet(host_float vol)
{
    wetFader.setMix(vol);
}

void Delay::processBlock(DSPObject *obj)
{
    Delay *self = static_cast<Delay *>(obj);

    self->processBlock();
}
