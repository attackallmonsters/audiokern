#include "FastTan.h"

FastTan::FastTan()
{
    registerBlockProcessor(&FastTan::processBlock);
}
void FastTan::initialize()
{
    outputBufferL.initialize(DSP::blockSize);
    outputBufferR.initialize(DSP::blockSize);
}

void FastTan::processBlock(DSPObject *dsp)
{
    FastTan *ft = static_cast<FastTan *>(dsp);

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        (ft->outputBufferL)[i] = fast_tanh((ft->outputBufferL)[i]);
        (ft->outputBufferR)[i] = fast_tanh((ft->outputBufferR)[i]);
    }
}