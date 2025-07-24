#pragma once

#include "DSPObject.h"
#include "DSPBusManager.h"

/**
 * @brief Base class for all sound-generating DSP objects.
 * 
 * Provides automatic connection to output and FM audio busses
 * based on naming convention. Subclasses override initializeGenerator().
 */
class SoundGenerator : public DSPObject
{
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

    /// Connected audio output bus (e.g. "out")
    DSPAudioBus* outputBus = nullptr;

    /// Optional FM modulation input bus (e.g. "fm")
    DSPAudioBus* fmBus = nullptr;
};
