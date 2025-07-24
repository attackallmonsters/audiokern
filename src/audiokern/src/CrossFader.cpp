#include "CrossFader.h"

CrossFader::CrossFader()
{
    registerBlockProcessor(&CrossFader::processBlock);
}

DSPUsage CrossFader::initializeObject()
{
    slew.initialize("slew" + getName());
    slew.setSlewTime(1.0);

    inputBufferAL.initialize("inputBufferAL" + getName(), DSP::blockSize);
    inputBufferAR.initialize("inputBufferAR" + getName(), DSP::blockSize);
    
    inputBufferBL.initialize("inputBufferBL" + getName(), DSP::blockSize);
    inputBufferBR.initialize("inputBufferBR" + getName(), DSP::blockSize);

    setMix(0.0);

    return DSPUsage::OutputOnly;
}

void CrossFader::audioInputForA(DSPObject &dspObject)
{
    DSPSignalBus bus = dspObject.outputSignalBus;

    inputBufferAL = *bus.left;
    inputBufferAR = *bus.right;
}

void CrossFader::audioInputForA(DSPSignalBus &signalBus)
{
    inputBufferAL = *signalBus.left;
    inputBufferAR = *signalBus.right;
}

void CrossFader::audioInputForA(DSPSampleBuffer &bufferL, DSPSampleBuffer &bufferR)
{
    inputBufferAL = bufferL;
    inputBufferAR = bufferR;
}

void CrossFader::audioInputForB(DSPObject &dspObject)
{
    DSPSignalBus bus = dspObject.outputSignalBus;

    inputBufferBL = *bus.left;
    inputBufferBR = *bus.right;
}

void CrossFader::audioInputForB(DSPSignalBus &signalBus)
{
    inputBufferBL = *signalBus.left;
    inputBufferBR = *signalBus.right;
}

void CrossFader::audioInputForB(DSPSampleBuffer &bufferL, DSPSampleBuffer &bufferR)
{
    inputBufferBL = bufferL;
    inputBufferBR = bufferR;
}

void CrossFader::setMix(double value)
{
    // Clamp between 0.0 and 1.0
    mix = clamp(value, 0.0, 1.0);
    slew.setTarget(mix);
}

void CrossFader::processBlock()
{
    dsp_float gainA, gainB;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        dsp_math::get_sin_cos(slew.process() * 0.5 * dsp_math::DSP_PI, &gainB, &gainA);

        outputBufferL[i] = inputBufferAL[i] * gainA + inputBufferBL[i] * gainB;
        outputBufferR[i] = inputBufferAR[i] * gainA + inputBufferBR[i] * gainB;
    }
}

void CrossFader::processBlock(DSPObject *dsp)
{
    CrossFader *self = static_cast<CrossFader *>(dsp);
    self->processBlock();
}
