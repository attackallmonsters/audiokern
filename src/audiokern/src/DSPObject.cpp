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
    usage = initializeComponent();
    setUsage();
}

void DSPObject::initialize(size_t count)
{
    usage = initializeComponent(count);
    setUsage();
}

void DSPObject::setUsage()
{
    switch (usage)
    {
    case DSPObjectUsage::Audio:
        inputBufferL.create(DSP::blockSize);
        inputBufferR.create(DSP::blockSize);
        outputBufferL.create(DSP::blockSize);
        outputBufferR.create(DSP::blockSize);
        break;
    case DSPObjectUsage::FM:
        fmBufferL.create(DSP::blockSize);
        fmBufferR.create(DSP::blockSize);
        outputBufferL.create(DSP::blockSize);
        outputBufferR.create(DSP::blockSize);
        break;
    case DSPObjectUsage::OutputOnly:
        outputBufferL.create(DSP::blockSize);
        outputBufferR.create(DSP::blockSize);
        break;
    case DSPObjectUsage::Process:
        processBufferL.create(DSP::blockSize);
        processBufferR.create(DSP::blockSize);
        modulationBuffer.create(DSP::blockSize);
        break;
    case DSPObjectUsage::Modulation:
        modulationBuffer.create(DSP::blockSize);
        break;
    case DSPObjectUsage::None:
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
    case DSPObjectUsage::Audio:
    case DSPObjectUsage::OutputOnly:
    case DSPObjectUsage::FM:
        if (usage == DSPObjectUsage::Process)
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
    case DSPObjectUsage::Process:
        if (usage == DSPObjectUsage::Process)
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
    case DSPObjectUsage::Modulation:
        throw std::runtime_error("Cannot use modulation source as input");
        break;
    case DSPObjectUsage::None:
        throw std::runtime_error("Cannot use undefined source (DSPObjectUsage::None) as input");
        break;
    default:
        throw std::runtime_error("DSPObject::audioInputFrom unknown option");
        break;
    }
}

void DSPObject::fmInputFrom(DSPObject &dspObject)
{
    // currently uses input buffers for modulation
    if (dspObject.usage != DSPObjectUsage::Audio &&
        dspObject.usage != DSPObjectUsage::OutputOnly &&
        usage != DSPObjectUsage::FM)
    {
        throw std::runtime_error("Invalid usage: modulation source must be DSPObjectUsage::Audio or DSPObjectUsage::OutputOnly, this must be DSPObjectUsage::FM");
    }

    fmBufferL = dspObject.outputBufferL;
    fmBufferR = dspObject.outputBufferR;
}

void DSPObject::modulationInputFrom(DSPObject &dspObject)
{
    // currently uses input buffers for modulation
    if (dspObject.usage != DSPObjectUsage::Modulation && usage != DSPObjectUsage::Process)
    {
        throw std::runtime_error("Invalid usage: modulation source must be DSPObjectUsage::Modulation, this must be DSPObjectUsage::Process");
    }

    modulationBuffer = dspObject.modulationBuffer;
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

DSPObjectUsage DSPObject::initializeComponent()
{
    return DSPObjectUsage::None;
}

DSPObjectUsage DSPObject::initializeComponent(size_t /*count*/)
{
    return DSPObjectUsage::None;
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