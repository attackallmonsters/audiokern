#pragma once

#include <stddef.h>
#include <stdexcept>
#include "clamp.h"
#include "DSPSampleBuffer.h"
#include "omfg.h"

class DSP; // Forward declaration

class DSPObject : public DSP
{
public:
    // Ctor
    DSPObject();

    // Dtor
    virtual ~DSPObject();

    // Gets the DSP object name
    const std::string &getName() const { return objectName; }

    // Initializes a DSP-Object
    void initialize(const std::string &name);

    // Initializes the DSP objectwith an amount of internal objects
    void initialize(const std::string &name, size_t count);

    // Finalization of initialization process
    void finalize();

    // Calculates the next sample buffer
    void process();

protected:
    // Avoid vtable lookup, implementation on derived class
    using BlockProcessor = void (*)(DSPObject *);

    // Derived class registers the function for sample block generation
    void registerBlockProcessor(BlockProcessor f);

    // Initializes the component
    virtual void initializeObject();

    // Initializes the component with an amount of internal objects
    virtual void initializeObject(size_t count);

private:
    // Dummy SampleFunc for setSamples
    static void defaultBlockProcess(DSPObject *);

    // Sample generation from derived class
    BlockProcessor processBlockFunc;

    // Sample counter
    static long elapsedSamples;

    // The object name
    std::string objectName;
};