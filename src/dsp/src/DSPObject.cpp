#include "clamp.h"
#include "DSPObject.h"
#include <cstring>

// Contructor
DSPObject::DSPObject()
{
    processBlockFunc = defaultBlockProcess;
}

// Destructor
DSPObject::~DSPObject()
{
}

// Initializes a DSP-Object
void DSPObject::initialize()
{
}

// Generates the next audio sample block
void DSPObject::generateBlock()
{
    (*processBlockFunc)(this);
}

void DSPObject::registerBlockProcessor(BlockProcessor f)
{
    processBlockFunc = f;
}

// Dummy block process
void DSPObject::defaultBlockProcess(DSPObject *)
{
}