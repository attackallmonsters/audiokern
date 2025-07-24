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
    /**
     * @brief Sets the name of the modulation bus to be used by this object.
     *
     * This method must be called before initialization. The bus will be resolved
     * during initializeObject() by querying the DSPBusManager.
     *
     * @param busName The name of the modulation bus (e.g., "mod_wet").
     */
    void setModulationBus(const std::string &busName);

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
     * @brief The connected input audio bus.
     */
    DSPAudioBus *inputBus = nullptr;

    /**
     * @brief The connected wet output bus.
     */
    DSPAudioBus *wetBus = nullptr;

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
