#pragma once

#include "DSPObject.h"
#include "DSPBusManager.h"

/**
 * @class Modulator
 * @brief DSPObject for modulating audio signals.
 *
 * The Modulator class represents a signal processor that applies modulation
 * effects to audio sample buffers. It is derived from DSPObject and can be
 * integrated into the audio processing graph.
 */
class Modulator : public DSPObject
{
public:
    /**
     * @brief Connects this DSP object to the modulation buffer of a named modulation bus.
     *
     * This assigns the internal modulation bus pointer to the DSPModulationBus associated with the given name.
     * The object will then receive its modulation input from this bus during processing.
     *
     * Example usage:
     *     connectToModulationBus("mod_cutoff");
     *
     * @param busName The name of the modulation bus to connect to.
     * @throws std::runtime_error if no such bus exists.
     */
    void connectToModulationBus(const std::string &busName);

    /**
     * @brief Connects this DSP object to the audio processing buffer of a named bus.
     *
     * Looks up the DSPAudioBus with the given name using DSPBusManager.
     * After this call, the object will read audio data from the associated bus.
     *
     * @param busName The name of the audio bus to connect to (e.g. "input", "osc1_output").
     * @throws std::runtime_error if the bus with the given name does not exist.
     */
    void connectToProcessBuffer(const std::string &busName);

    /**
     * @brief Multiplies the internal modulation signal with the given buffers. Executes process!
     *
     * This method applies a sample-wise multiplication between an internal modulation
     * signal (typically generated or stored within the Modulator) and the provided
     * left and right audio sample buffers. This is commonly used for amplitude
     * modulation (e.g. ring modulation) or applying an envelope or LFO shape to a signal.
     *
     * @param bufL Reference to the left channel sample buffer to be modulated.
     * @param bufR Reference to the right channel sample buffer to be modulated.
     */
    void multiply();

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
     * @brief Subclass-specific modulator setup logic.
     */
    virtual void initializeModulator();

    /**
     * @brief Subclass-specific modulator setup with voice count.
     *
     * @param count Number of internal instances (e.g., polyphonic LFOs).
     */
    virtual void initializeModulator(size_t count);

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
     * @brief Called when an processing audio bus has been successfully connected.
     *
     * Derived classes can override this method to perform setup or adjustments
     * after the processing bus connection is established (e.g., update processing buffer references).
     *
     * Default implementation does nothing.
     */
    virtual void onProcessBusConnected();

    /**
     * @brief Modulation output bus 
     */
    DSPModulationBus *modulationBus = nullptr;

    /**
     * @brief Audio bus created for modulation functions (multiply, etc.)
     */
    DSPAudioBus *processBus;
};
