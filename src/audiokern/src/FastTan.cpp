#include "FastTan.h"

FastTan::FastTan()
{
    registerBlockProcessor(&FastTan::processBlock);
}

void FastTan::processBlock()
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        processBus->l[i] = dsp_math::fast_tanh(processBus->l[i]);
        processBus->r[i] = dsp_math::fast_tanh(processBus->r[i]);
    }
}

void FastTan::processBlock(DSPObject *dsp)
{
    FastTan *self = static_cast<FastTan *>(dsp);
    self->processBlock();
}