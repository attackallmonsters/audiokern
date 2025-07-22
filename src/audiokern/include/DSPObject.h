#pragma once

#include <stddef.h>
#include <stdexcept>
#include "clamp.h"
#include "DSP.h"
#include "DSPSampleBuffer.h"

struct DSPBus
{
    DSPSampleBuffer *left;
    DSPSampleBuffer *right;

    DSPBus() = default;
    DSPBus(DSPSampleBuffer *l, DSPSampleBuffer *r) : left(l), right(r) {};
};

enum class DSPUsage
{
    Audio,      // has input L/R, output L/R buffers
    Process,    // has process L/R, modulation L/R buffers
    Modulation, // has a modulation buffer
    OutputOnly, // has output L/R buffers only
    FM,         // has output L/R, fm L/R buffers
    Effect,     // has input L/R, output L/R, modulation buffer
    None        // no buffers at all
};

class DSPObject : public DSP
{
public:
    // Ctor
    DSPObject();

    // Dtor
    virtual ~DSPObject();

    // Initializes a DSP-Object
    void initialize();

    // Initializes the DSP objectwith an amount of internal objects
    void initialize(size_t count);

    // Connects the output of DSP object with the input of this object
    void audioInputFrom(DSPObject &dspObject);

    // Connects the output of DSP object with the modulation buffer of this object
    void fmInputFrom(DSPObject &dspObject);

    // Connects a modulators output to a processing component (filter etc.)
    void modulationInputFrom(DSPObject &dspObject);

    // Connects the DSP object to the audio host output buffer
    void hostOutputTo(host_float *outL, host_float *outR);

    // Gets the output buffers
    DSPBus getOutputBus();

    // Gets the input buffers
    DSPBus getInputBus();

    // Gets the process buffers
    DSPBus getProcessBus();

    // Calculates the next sample buffer
    void generateBlock();

protected:
    // Avoid vtable lookup, implementation on derived class
    using BlockProcessor = void (*)(DSPObject *);

    // Derived class registers the function for sample block generation
    void registerBlockProcessor(BlockProcessor f);

    // Initializes the component
    virtual DSPUsage initializeObject();

    // Initializes the component with an amount of internal objects
    virtual DSPUsage initializeObject(size_t count);

    // Called when buffers have been created
    virtual void onBuffersCreated();

    // Input buffers
    DSPSampleBuffer inputBufferL;
    DSPSampleBuffer inputBufferR;

    // Output buffers
    DSPSampleBuffer outputBufferL;
    DSPSampleBuffer outputBufferR;

    // FM buffers
    DSPSampleBuffer fmBufferL;
    DSPSampleBuffer fmBufferR;

    // Process buffers (used for direct buffer manipulation like filters, etc)
    DSPSampleBuffer processBufferL;
    DSPSampleBuffer processBufferR;

    // Modulation buffers (used for modulation)
    DSPSampleBuffer modulationBuffer;

private:
    // How to connect and use the buffers
    DSPUsage usage = DSPUsage::None;

    // Will clean up buffers after initializing
    void setUsage();

    // Dummy SampleFunc for setSamples
    static void defaultBlockProcess(DSPObject *);

    // Sample generation from derived class
    BlockProcessor processBlockFunc;

    // Sample counter
    static long elapsedSamples;
};