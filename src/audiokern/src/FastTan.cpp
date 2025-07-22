#include "FastTan.h"

FastTan::FastTan()
{
    registerBlockProcessor(&FastTan::processBlock);
}

DSPObjectUsage FastTan::initializeComponent()
{
    return DSPObjectUsage::Process;
}

void FastTan::processBlock(DSPObject *dsp)
{
    FastTan *ft = static_cast<FastTan *>(dsp);

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        (ft->processBufferL)[i] = dsp_math::fast_tanh((ft->processBufferL)[i]);
        (ft->processBufferR)[i] = dsp_math::fast_tanh((ft->processBufferR)[i]);
    }
}