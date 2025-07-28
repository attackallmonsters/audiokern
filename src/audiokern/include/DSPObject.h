#pragma once

#include <stddef.h>
#include <stdexcept>
#include "clamp.h"
#include "DSPSampleBuffer.h"
#include "omfg.h"

class DSP; // Forward declaration

/**
 * @brief Abstract base class for all DSP components.
 *
 * DSPObject provides a consistent initialization and processing interface
 * for derived signal processing modules. It allows registration of a block
 * processing function and manages a name and sample tracking.
 */
class DSPObject : public DSP
{
public:
    /**
     * @brief Constructs an uninitialized DSPObject.
     */
    DSPObject();

    /**
     * @brief Virtual destructor.
     */
    virtual ~DSPObject();

    /**
     * @brief Returns the name of the DSP object.
     * @return Constant reference to the object name.
     */
    const std::string &getName() const { return objectName; }

    /**
     * @brief Initializes the DSP object with a given name.
     * @param name The name to assign.
     */
    void initialize(const std::string &name);

    /**
     * @brief Initializes the DSP object with a name and internal element count.
     * @param name The name to assign.
     * @param count Number of internal DSP elements to initialize.
     */
    void initialize(const std::string &name, size_t count);

    /**
     * @brief Finalizes the initialization of the DSP object.
     *
     * This is called after `initialize()` to complete setup (e.g. allocate buffers).
     */
    void finalize();

    /**
     * @brief Performs the DSP processing for one sample block.
     *
     * This function internally dispatches to the registered block processor.
     */
    void process();

protected:
    /**
     * @brief Typedef for the block processing function.
     *
     * A derived class registers this function to define how the block is processed.
     */
    using BlockProcessor = void (*)(DSPObject *);

    /**
     * @brief Registers the block processor function.
     * @param f Function pointer to the block processing function.
     */
    void registerBlockProcessor(BlockProcessor f);

    /**
     * @brief Optional virtual method for object-specific initialization.
     *
     * Called by `initialize()` in derived classes.
     */
    virtual void initializeObject();

    /**
     * @brief Optional virtual method for object-specific initialization with count.
     *
     * Called by `initialize(name, count)` in derived classes.
     * @param count Number of internal objects to initialize.
     */
    virtual void initializeObject(size_t count);

private:
    /**
     * @brief Default (empty) block processor used if none is registered.
     * @param obj Pointer to this DSPObject.
     */
    static void defaultBlockProcess(DSPObject *obj);

    /**
     * @brief Function pointer to the current block processor.
     */
    BlockProcessor processBlockFunc;

    /**
     * @brief Global static sample counter shared across all DSPObjects.
     */
    static long elapsedSamples;

    /**
     * @brief Name assigned to this DSP object.
     */
    std::string objectName;
};
