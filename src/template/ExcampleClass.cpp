#include "ExcampleClass.h"

ExcampleClass::ExcampleClass()
{
    registerBlockProcessor(&ExcampleClass::processBlock);
}

ExcampleClass::~ExcampleClass()
{
}

void ExcampleClass::processBlock()
{
    for (int i = 0; i < DSP::blockSize; i++)
    {
        // creation of samples, modulation, etc.
    }
}

void ExcampleClass::processBlock(DSPObject *dsp)
{
    ExcampleClass *self = static_cast<ExcampleClass *>(dsp);
    self->processBlock();
}