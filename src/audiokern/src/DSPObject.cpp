#include "DSPObject.h"

DSPObject::DSPObject()
{
    processBlockFunc = defaultBlockProcess;
}

DSPObject::~DSPObject()
{
}

void DSPObject::initialize()
{
    usage = initializeObject();
    setUsage();
}

void DSPObject::initialize(size_t count)
{
    usage = initializeObject(count);
    setUsage();
}

void DSPObject::setUsage()
{
    switch (usage)
    {
    case DSPUsage::Audio:
        inputBufferL.create(DSP::blockSize);
        inputBufferR.create(DSP::blockSize);
        outputBufferL.create(DSP::blockSize);
        outputBufferR.create(DSP::blockSize);
        break;
    case DSPUsage::Effect:
        inputBufferL.create(DSP::blockSize);
        inputBufferR.create(DSP::blockSize);
        outputBufferL.create(DSP::blockSize);
        outputBufferR.create(DSP::blockSize);
        modulationBuffer.create(DSP::blockSize);
        break;
    case DSPUsage::FM:
        fmBufferL.create(DSP::blockSize);
        fmBufferR.create(DSP::blockSize);
        outputBufferL.create(DSP::blockSize);
        outputBufferR.create(DSP::blockSize);
        break;
    case DSPUsage::OutputOnly:
        outputBufferL.create(DSP::blockSize);
        outputBufferR.create(DSP::blockSize);
        break;
    case DSPUsage::Process:
        processBufferL.create(DSP::blockSize);
        processBufferR.create(DSP::blockSize);
        modulationBuffer.create(DSP::blockSize);
        break;
    case DSPUsage::Modulation:
        modulationBuffer.create(DSP::blockSize);
        break;
    case DSPUsage::None:
        return;
        break;
    default:
        throw std::runtime_error("DSPObject::audioInputFrom unknown option");
        break;
    }

    onBuffersCreated();
}

void DSPObject::audioInputFrom(DSPObject &dspObject)
{
    switch (dspObject.usage)
    {
    case DSPUsage::Audio:
    case DSPUsage::OutputOnly:
    case DSPUsage::FM:
    case DSPUsage::Effect:
        if (usage == DSPUsage::Process)
        {
            processBufferL = dspObject.outputBufferL;
            processBufferR = dspObject.outputBufferR;
        }
        else
        {
            inputBufferL = dspObject.outputBufferL;
            inputBufferR = dspObject.outputBufferR;
        }
        break;
    case DSPUsage::Process:
        if (usage == DSPUsage::Process)
        {
            processBufferL = dspObject.processBufferL;
            processBufferR = dspObject.processBufferR;
        }
        else
        {
            inputBufferL = dspObject.processBufferL;
            inputBufferR = dspObject.processBufferR;
        }
        break;
    case DSPUsage::Modulation:
        throw std::runtime_error("Cannot use modulation source as input");
        break;
    case DSPUsage::None:
        throw std::runtime_error("Cannot use undefined source (DSPUsage::None) as input");
        break;
    default:
        throw std::runtime_error("DSPObject::audioInputFrom unknown option");
        break;
    }
}

void DSPObject::fmInputFrom(DSPObject &dspObject)
{
    // currently uses input buffers for modulation
    if ((dspObject.usage == DSPUsage::Audio || dspObject.usage == DSPUsage::OutputOnly || dspObject.usage == DSPUsage::FM) && usage == DSPUsage::FM)
    {
        fmBufferL = dspObject.outputBufferL;
        fmBufferR = dspObject.outputBufferR;
        return;
    }

    throw std::runtime_error("Invalid usage: modulation source must be DSPUsage::Audio or DSPUsage::OutputOnly, this must be DSPUsage::FM");
}

void DSPObject::modulationInputFrom(DSPObject &dspObject)
{
    // currently uses input buffers for modulation
    if (dspObject.usage == DSPUsage::Modulation && (usage == DSPUsage::Process || usage == DSPUsage::Effect))
    {
        modulationBuffer = dspObject.modulationBuffer;
        return;
    }

    throw std::runtime_error("Invalid usage: modulation source must be DSPUsage::Modulation, this must be DSPUsage::Process");
}

void DSPObject::hostOutputTo(host_float *outL, host_float *outR)
{
    outputBufferL = outL;
    outputBufferR = outR;
}

DSPBus DSPObject::getOutputBus()
{
    return DSPBus(&outputBufferL, &outputBufferR);
}

DSPBus DSPObject::getInputBus()
{
    return DSPBus(&inputBufferL, &inputBufferR);
}

DSPBus DSPObject::getProcessBus()
{
    return DSPBus(&processBufferL, &processBufferR);
}

DSPUsage DSPObject::initializeObject()
{
    return DSPUsage::None;
}

DSPUsage DSPObject::initializeObject(size_t /*count*/)
{
    return DSPUsage::None;
}

void DSPObject::onBuffersCreated()
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