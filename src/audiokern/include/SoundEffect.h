#pragma once

#include "DSPObject.h"
#include "DSPBusManager.h"
#include "CrossFader.h"

/**
 * @brief Base class for all sound processing DSP effects.
 *
 * Automatically connects to standard input/output buses and
 * a named modulation bus for parameter control.
 */
class SoundEffect : public DSPObject
{
public:
    SoundEffect();

    /**
     * @brief Connects this DSP object to the input buffer of a named audio bus.
     *
     * This assigns the internal inputBus pointer to the DSPAudioBus associated with the given name.
     * The object will then receive its audio input from this bus during processing.
     *
     * Example usage:
     *     connectInputToBus(oscOutputBus);
     *
     * @param bus The audio input bus to connect to.
     * @throws std::runtime_error if no such bus exists.
     */
    void connectInputToBus(DSPAudioBus &bus);

    /**
     * @brief Connects this DSP object to the output or wet buffer of a named audio bus.
     *
     * This assigns the internal outputBus pointer to the DSPAudioBus associated with the given name.
     * All generated or processed audio will be written to this bus.
     *
     * Example usage:
     *     connectOutputToBus(bus);
     *
     * @param bus The audio output bus to connect to.
     * @throws std::runtime_error if no such bus exists.
     */
    void connectOutputToBus(DSPAudioBus &bus);

    /**
     * @brief Sets an audio without using DSPBusManager.
     *
     * This assigns the internal outputBus pointer to the DSPAudioBus.
     * All generated or processed audio will be written to this bus.
     *
     * Example usage:
     *     setOutputBus(outputBus);
     *
     * @param bus THe bus to use as output buss.
     */
    void setOutputBus(DSPAudioBus &bus);

    /**
     * @brief Sets the name of the modulation bus to be used by this object.
     *
     * This method must be called before initialization. The bus will be resolved
     * during initializeObject() by querying the DSPBusManager.
     *
     * @param bus The modulation bus to connect to.
     */
    void connectModulationToBusA(DSPModulationBus &bus);

    /**
     * @brief Sets the name of the modulation bus to be used by this object.
     *
     * This method must be called before initialization. The bus will be resolved
     * during initializeObject() by querying the DSPBusManager.
     *
     * @param bus The modulation bus to connect to.
     */
    void connectModulationToBusB(DSPModulationBus &bus);

    /**
     * @brief Sets the output wet signal level.
     * @param vol Volume factor [0.0, 1.0].
     */
    void setWet(host_float vol);

protected:
    /**
     * @brief Registers the effect block processor function.
     * @param f Function pointer to the block processing function.
     */
    void registerEffectBlockProcessor(BlockProcessor f);

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
    virtual void onInputBusConnected(DSPAudioBus &bus);

    /**
     * @brief Called when an output audio bus has been successfully connected.
     *
     * This hook is useful for reconfiguring buffer pointers or clearing states that
     * depend on output availability.
     *
     * Default implementation does nothing.
     */
    virtual void onOutputBusConnected(DSPAudioBus &bus);

    /**
     * @brief Called when a modulation bus has been successfully connected.
     *
     * If the object supports modulation (e.g., LFOs, filters, FM carriers), this method
     * can be overridden to react to modulation signal routing.
     *
     * Default implementation does nothing.
     */
    virtual void onModulationBusAConnected(DSPModulationBus &bus);

    /**
     * @brief Called when a modulation bus has been successfully connected.
     *
     * If the object supports modulation (e.g., LFOs, filters, FM carriers), this method
     * can be overridden to react to modulation signal routing.
     *
     * Default implementation does nothing.
     */
    virtual void onModulationBusBConnected(DSPModulationBus &bus);

    /**
     * @brief Called when a wet bus has been successfully connected.
     *
     * If the object supports modulation (e.g., LFOs, filters, FM carriers), this method
     * can be overridden to react to modulation signal routing.
     *
     * Default implementation does nothing.
     */
    virtual void onWetBusConnected(DSPAudioBus &bus);

    /**
     * @brief The connected input audio bus.
     */
    DSPAudioBus inputBus;

    /**
     * @brief The connected wet output bus.
     */
    DSPAudioBus outputBus;

    /**
     * @brief The optional connected modulation bus A.
     */
    DSPModulationBus modulationBusA;

    /**
     * @brief The optional connected modulation bus B.
     */
    DSPModulationBus modulationBusB;

    /**
     * @brief The default wet signal bus
     */
    DSPAudioBus wetBus;

private:
    /**
     * @brief Static DSP processing callback.
     * Dispatches to the instance-level `processBlock()`.
     */
    static void processBlock(DSPObject *dsp);

    /**
     * @brief Function pointer to the current block processor.
     */
    BlockProcessor effectProcessBlockFunc;

    /// @brief Output wet level
    host_float wet;

    /// @brief Crossfader for dry/wet mixing
    CrossFader wetFader;
};
