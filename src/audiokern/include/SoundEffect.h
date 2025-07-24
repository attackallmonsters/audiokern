#pragma once

#include "DSPObject.h"
#include "DSPBusManager.h"

/**
 * @brief Base class for all sound processing DSP effects.
 *
 * Automatically connects to standard input/output buses and
 * a named modulation bus for parameter control.
 */
class SoundEffect : public DSPObject
{
public:
    /**
     * @brief Connects this DSP object to the input buffer of a named audio bus.
     *
     * This assigns the internal inputBus pointer to the DSPAudioBus associated with the given name.
     * The object will then receive its audio input from this bus during processing.
     *
     * Example usage:
     *     connectToInputBus("osc1_output");
     *
     * @param busName The name of the audio input bus to connect to.
     * @throws std::runtime_error if no such bus exists.
     */
    void connectToInputBus(const std::string &busName);

    /**
     * @brief Connects this DSP object to the output or wet buffer of a named audio bus.
     *
     * This assigns the internal outputBus pointer to the DSPAudioBus associated with the given name.
     * All generated or processed audio will be written to this bus.
     *
     * Example usage:
     *     connectToOutputBus("wet_output");
     *
     * @param busName The name of the audio output bus to connect to.
     * @throws std::runtime_error if no such bus exists.
     */
    void connectToOutputBus(const std::string &busName);

    /**
     * @brief Sets the name of the modulation bus to be used by this object.
     *
     * This method must be called before initialization. The bus will be resolved
     * during initializeObject() by querying the DSPBusManager.
     *
     * @param busName The name of the modulation bus (e.g., "mod_wet").
     */
    void connectToModulationBus(const std::string &busName);

protected:
    /**
     * @brief Finalized initialization without voice count.
     */
    void initializeObject() override final;

    /**
     * @brief Finalized initialization with voice count.
     */
    void initializeObject(size_t count) override final;

    /**
     * @brief Subclass-specific initialization.
     */
    virtual void initializeEffect();

    /**
     * @brief Subclass-specific initialization with voice count.
     *
     * @param count Number of internal processing instances (e.g. voices).
     */
    virtual void initializeEffect(size_t count);

    /**
     * @brief Called when an input audio bus has been successfully connected.
     *
     * Derived classes can override this method to perform setup or adjustments
     * after the input bus connection is established (e.g., update input buffer references).
     *
     * Default implementation does nothing.
     */
    virtual void onInputBusConnected();

    /**
     * @brief Called when an output audio bus has been successfully connected.
     *
     * This hook is useful for reconfiguring buffer pointers or clearing states that
     * depend on output availability.
     *
     * Default implementation does nothing.
     */
    virtual void onOutputBusConnected();

    /**
     * @brief Called when a modulation bus has been successfully connected.
     *
     * If the object supports modulation (e.g., LFOs, filters, FM carriers), this method
     * can be overridden to react to modulation signal routing.
     *
     * Default implementation does nothing.
     */
    virtual void onModulationBusConnected();

    /**
     * @brief The connected input audio bus.
     */
    DSPAudioBus *inputBus = nullptr;

    /**
     * @brief The connected wet output bus.
     */
    DSPAudioBus *outputBus = nullptr;

    /**
     * @brief The optional connected modulation bus.
     */
    DSPModulationBus *modulationBus = nullptr;

private:
    /**
     * @brief Indicates whether a modulation bus should be used for this object.
     *
     * If set to true, the DSP object will attempt to resolve and connect a modulation
     * bus during initialization. If false, no modulation bus will be connected.
     */
    bool modulationBusEnabled = false;
};
