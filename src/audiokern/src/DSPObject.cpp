#include "DSPObject.h"
#include "DSP.h"
#include "DSPBusManager.h"

DSPObject::DSPObject()
{
    processBlockFunc = defaultBlockProcess;
}

DSPObject::~DSPObject()
{
}

void DSPObject::initialize(const std::string &name)
{
    objectName = "_" + name;
    initializeObject();

    DSP::registerObject(*this);
}

void DSPObject::initialize(const std::string &name, size_t count)
{
    objectName = "_" + name;
    initializeObject(count);
}

void DSPObject::finalize()
{
}

void DSPObject::initializeObject()
{
}

void DSPObject::initializeObject(size_t /*count*/)
{
}

// Generates the next audio sample block
void DSPObject::process()
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