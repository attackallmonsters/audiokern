#pragma once

#include "DSPBusManager.h"
#include "DSPSampleBuffer.h"
#include "DSPObject.h"
#include <vector>
#include <cstddef>

/**
 * @brief The Mixer class sums multiple audio input busses into a single output bus.
 *
 * Each instance dynamically registers its input busses based on the given count.
 * The input signals are expected to be written externally into the named busses.
 * The mixer reads all input buffers and sums them into a shared stereo output.
 */
class Mixer : public DSPObject
{
public:
    // Constructor
    Mixer();

    /**
     * @brief Returns the name of an input bus by index.
     *
     * These names are automatically generated in initializeObject()
     * and must be used by external sources to write to the mixer's inputs.
     *
     * @param index Index of the input channel
     * @return Name of the registered input bus
     */
    DSPAudioBus &getInputBus(size_t index);

    /**
     * @brief Connects the mixer's output to a named DSP bus.
     *
     * The mixed result will be written to the given output bus on each block.
     *
     * @param busName Name of the output bus to connect to
     */
    void connectOutputToBus(DSPAudioBus &bus);

protected:
    /**
     * @brief Initializes the mixer instance and registers input busses.
     *
     * This function is called during setup and creates `count` uniquely named busses.
     * Each bus will later be used to read external audio signals for mixing.
     *
     * @param count Number of input channels to mix
     */
    void initializeObject(size_t count) override;

private:
    /**
     * @brief Static DSP callback wrapper.
     *
     * This is required by the DSPObject interface and delegates to the instance method.
     */
    static void processBlock(DSPObject *dsp);

    /**
     * @brief Processes one audio block.
     *
     * Reads all connected input busses and accumulates them into the output bus.
     */
    void processBlock();

    size_t busCount;                   // Number of input busses
    std::vector<DSPAudioBus *> busses; // The actual input buffers
    DSPAudioBus outputBus;             // Output bus where the mixed signal is written
};
