#pragma once

#include <stddef.h>
#include "DSP.h"
#include "DSPBuffer.h"

class DSPObject : public DSP
{
public:
    // Ctor
    DSPObject();

    // Dtor
    virtual ~DSPObject();

    // Initializes a DSP-Object
    virtual void initialize();

    // Calculates the next sample buffer
    void generateBlock();

protected:
    // Avoid vtable lookup, implementation on derived class
    using BlockProcessor = void (*)(DSPObject *);

    // Derived class registers the function for sample block generation
    void registerBlockProcessor(BlockProcessor f);

private:
    // Dummy SampleFunc for setSamples
    static void defaultBlockProcess(DSPObject *);

    // Sample generation from derived class
    BlockProcessor processBlockFunc;
};