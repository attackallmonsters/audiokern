
#pragma once

#include "DSPObjectCollection.h"
#include "DSPSampleBuffer.h"
#include "omfg.h"
#include "Busses.h"
#include <stdexcept>

/**
 * @brief Static manager for registering and accessing named audio and modulation buses.
 *
 * This class provides static registration and retrieval of DSP buses by name.
 * All buses are allocated statically to ensure Organelle compatibility (no heap allocation).
 * Must be initialized after DSP::blockSize is known.
 */
class DSPBusManager
{
public:
    /**
     * @brief Initializes the DSP bus manager
     */
    void initialize();

    /**
     * @brief Registers a new audio channel with the given name.
     *
     * Ensures uniqueness of channel names. If a channel with the same name
     * already exists, the registration is silently ignored.
     * Throws if the maximum number of audio channels is exceeded.
     *
     * @param name The unique name of the audio channel to register.
     * @throws std::runtime_error if the channel limit is reached.
     */
    static DSPAudioBus &registerAudioBus(const std::string &name);

    /**
     * @brief Registers and connects an audio channel with external output buffers.
     *
     * Associates a previously registered audio channel (by name) with external output
     * buffers for the left and right channels. This is typically used to route audio
     * to a specific destination such as a hardware output or audio host like Pure Data,
     * SuperCollider, VST and so on.
     *
     * If the channel name has not been registered via the static
     * registerAudioBus(const std::string&) method, the call has no effect.
     *
     * @param name The name of the audio channel to bind.
     * @param outL Pointer to the left output buffer.
     * @param outR Pointer to the right output buffer.
     */
    static DSPAudioBus &registerAudioBus(const std::string &name, host_float *outL, host_float *outR);

    /**
     * @brief Registers and connects a modulation channel with an external output buffer.
     *
     * Associates a previously registered modulation channel (by name) with an external
     * output buffer. This is typically used to route low.mate control signals (e.g. LFOs,
     * envelopes, automation data) to external environments such as Pure Data, VST hosts,
     * or control.mate buses in SuperCollider.
     *
     * If the channel name has not been registered via the static
     * registerModulationBus(const std::string&) method, the call has no effect.
     *
     * @param name The name of the modulation channel to bind.
     * @param out Pointer to the control.mate output buffer.
     */
    static DSPModulationBus &registerModulationBus(const std::string &name, host_float *out);

    /**
     * @brief Registers a new modulation channel with the given name.
     *
     * Ensures that each modulation bus name is unique. If the name already exists,
     * the registration is skipped. Throws if the maximum number of modulation channels is reached.
     *
     * @param name The unique name of the modulation channel to register.
     * @throws std::runtime_error if the modulation channel limit is exceeded.
     */
    static DSPModulationBus &registerModulationBus(const std::string &name);

    /**
     * @brief Returns a pointer to the audio bus with the given name.
     * Returns nullptr if not found.
     *
     * @param name The name of the audio bus.
     * @return Pointer to DSPAudioBus or nullptr.
     */
    static DSPAudioBus &getAudioBus(const std::string &name);

    /**
     * @brief Returns a pointer to the modulation bus with the given name.
     * Returns nullptr if not found.
     *
     * @param name The name of the modulation bus.
     * @return Pointer to DSPModulationBus or nullptr.
     */
    static DSPModulationBus &getModulationBus(const std::string &name);

    /**
     * @brief Clears all registered bus names and resets counters.
     * Does not deallocate the static buffers.
     */
    static void clear();

    /**
     * @brief Validates that all registered buses are correctly initialized.
     * Throws std::runtime_error if any bus is invalid.
     * Only compiled in DEBUG builds.
     */
    static void validate();

    static void log();

    /**
     * @brief The name of a bus that exists as audio and modulation bus.
     * DSP objects can connect to this bus to have a buffer available.
     */
    const std::string nullBusName = "null";

private:
    // Audio bus storage
    static DSPObjectCollection<DSPAudioBus> audioBusses;

    // Modulation bus storage
    static DSPObjectCollection<DSPModulationBus> modulationBusses;
};
