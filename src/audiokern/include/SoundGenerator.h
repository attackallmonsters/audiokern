#pragma once

#include "DSPObject.h"
#include "DSPBusManager.h"

enum class GeneratorRole
{
    Normal,  // No FM, output only
    Carrier, // FM input
};

/**
 * @brief Base class for all sound-generating DSP objects.
 *
 * Provides automatic connection to output and FM audio busses
 * based on naming convention. Subclasses override initializeGenerator().
 */
class SoundGenerator : public DSPObject
{
public:
    /**
     * @brief Connects this DSP object to the FM buffer of a named audio bus.
     *
     * This assigns the internal fmBus pointer to the DSPAudioBus associated with the given name.
     * The object will then receive its audio FM input from this bus during processing.
     *
     * Example usage:
     *     connectToInputBus("osc1_output");
     *
     * @param busName The name of the audio input bus to connect to.
     * @throws std::runtime_error if no such bus exists.
     */
    void connectToFMBus(const std::string &busName);

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
     * @brief Sets the functional role of this sound generator.
     *
     * Determines whether the generator acts as a carrier (producing audio output),
     * a modulator (writing to a modulation bus), or both.
     * Must be called before initialization.
     *
     * @param role The role to assign (e.g., CarrierOnly, ModulatorOnly, CarrierAndModulator).
     */
    void setRole(GeneratorRole role);

protected:
    /**
     * @brief Finalized setup hook from DSPObject. Connects to standard buses.
     */
    void initializeObject() override final;

    /**
     * @brief Finalized setup hook from DSPObject with voice count.
     */
    void initializeObject(size_t count) override final;

    /**
     * @brief Subclass-specific generator initialization (no voice count).
     */
    virtual void initializeGenerator();

    /**
     * @brief Subclass-specific generator initialization (with voice count).
     */
    virtual void initializeGenerator(size_t count);

    /**
     * @brief Called when av FM input audio bus has been successfully connected.
     *
     * Derived classes can override this method to perform setup or adjustments
     * after the FM input bus connection is established (e.g., update input buffer references).
     *
     * Default implementation does nothing.
     */
    virtual void onFMBusConnected();

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
     * @brief Connected audio output bus
     */
    DSPAudioBus *outputBus = nullptr;

    /**
     * @brief Connected FM bus
     */
    DSPAudioBus *fmBus = nullptr;

    /**
     * @brief Role of the sound generator
     */
    GeneratorRole generatorRole = GeneratorRole::Carrier;
};
