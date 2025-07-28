#pragma once

#include "DSPObject.h"
#include "DSPBusManager.h"

/**
 * @brief Base class for audio signal processors (e.g. filters, limiters).
 *
 * Provides automatic connection to the standard output bus ("out").
 * Does not handle input or modulation buses. Subclasses override initializeProcessor().
 */
class SoundProcessor : public DSPObject
{
public:
    /**
     * @brief Connects this DSP object to the audio processing buffer of a named bus.
     *
     * Looks up the DSPAudioBus with the given name using DSPBusManager.
     * After this call, the object will read audio data from the associated bus.
     *
     * @param busName The name of the audio bus to connect to (e.g. "input", "osc1_output").
     * @throws std::runtime_error if the bus with the given name does not exist.
     */
    void connectToProcessBus(const std::string &busName);

    /**
     * @brief Connects this DSP object to the modulation buffer of a named bus.
     *
     * Looks up the DSPModulationBus with the given name using DSPBusManager.
     * After this call, the object will read modulation data (e.g., from LFOs or envelopes)
     * from the associated bus.
     *
     * @param busName The name of the modulation bus to connect to (e.g. "mod_cutoff").
     * @throws std::runtime_error if the bus with the given name does not exist.
     */
    void connectToModulationBus(const std::string &busName);

protected:
    /**
     * @brief Finalized DSPObject initialization without voice count.
     */
    void initializeObject() override final;

    /**
     * @brief Finalized DSPObject initialization with voice count.
     */
    void initializeObject(size_t count) override final;

    /**
     * @brief Subclass-specific processor initialization.
     */
    virtual void initializeProcessor();

    /**
     * @brief Subclass-specific processor initialization with voice count.
     *
     * @param count Number of internal processor instances (e.g. voices).
     */
    virtual void initializeProcessor(size_t count);

    /**
     * @brief Called when an processing audio bus has been successfully connected.
     *
     * Derived classes can override this method to perform setup or adjustments
     * after the processing bus connection is established (e.g., update processing buffer references).
     *
     * Default implementation does nothing.
     */
    virtual void onProcessBusConnected();

    /**
     * @brief Called when an modulation audio bus has been successfully connected.
     *
     * This hook is useful for reconfiguring buffer pointers or clearing states that
     * depend on modulation availability.
     *
     * Default implementation does nothing.
     */
    virtual void onModulationBusConnected();

    /**
     * @brief The connected audio processing bus.
     */
    DSPAudioBus *processBus = nullptr;

    /**
     * @brief The connected modulation bus.
     */
    DSPModulationBus *modulationBus = nullptr;
};
