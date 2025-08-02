#pragma once

#include "DSPObject.h"
#include "DSPBusManager.h"

/**
 * @class Modulator
 * @brief DSPObject for modulating audio or modulation signals.
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
     *     connectModulationToBus("mod_cutoff");
     *
     * @param busName The name of the modulation bus to connect to.
     * @throws std::runtime_error if no such bus exists.
     */
    void connectModulationToBus(DSPModulationBus &bus);

    /**
     * @brief Connects this DSP object to the modulation buffer of a named bus for FM modulation.
     *
     * @param busName The name of the modulation bus to connect to (e.g. "input", "lfo1_modulation").
     * @throws std::runtime_error if the bus with the given name does not exist.
     */
    void connectFMToBus(DSPModulationBus &bus);

    /**
     * @brief Disconnects the modulator from an FM modulation bus
     */
    void disconnectFMBus();

    /**
     * @brief Multiplies the internal modulation signal with the given audio bus buffers and stores
     * the results in the target bus buffers.
     *
     * This method applies a sample-wise multiplication between an internal modulation
     * signal (typically generated or stored within the Modulator) and the provided
     * bus sample buffers. 
     * 
     * @param bus Reference to an audio bus for sample multiplication
     */
    void processMultiply(DSPAudioBus &targetBus);

    /**
     * @brief Multiplies the internal modulation signal with the given modulation bus buffers and stores
     * the results in the target bus buffers.
     *
     * This method applies a sample-wise multiplication between an internal modulation
     * signal (typically generated or stored within the Modulator) and the provided
     * bus sample buffers. 
     * 
     * @param bus Reference to an modulation bus for value multiplication
     */
    void processMultiply(DSPModulationBus &targetBus);

    /**
     * @brief Sets the modulation gain
     *
     * @param g The gain factor on the process bus
     */
    void setGain(host_float g);

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
    virtual void onModulationBusConnected(DSPModulationBus &bus);

    /**
     * @brief Called when a modulation bus has been successfully connected for FM.
     *
     * Derived classes can override this method to perform setup or adjustments
     * after the processing bus connection is established (e.g., update processing buffer references).
     *
     * Default implementation does nothing.
     */
    virtual void onFMBusConnected(DSPModulationBus &bus);

    /**
     * @brief Called when a FM modulation bus has been disconnected
     *
     * Derived classes can override this method to perform setup or adjustments
     * after the processing bus connection is established (e.g., update processing buffer references).
     *
     * Default implementation does nothing.
     */
    virtual void onFMBusDisconnected(DSPModulationBus &bus);

    /**
     * @brief Modulation output bus 
     */
    DSPModulationBus modulationBus;

    /**
     * @brief Audio bus created for FM modulation
     */
    DSPModulationBus fmBus;

    /**
     * @brief Flag indicating if frequency modulation (FM) is enabled
     */bool fmEnabled;

    /**
     * @brief Gain factor for process bus
     */
    host_float gain;
};
