
#pragma once

#include "DSPSampleBuffer.h"
#include "omfg.h"
#include <stdexcept>

class DSPAudioBus
{
public:
    void initialize(const std::string &name, size_t size)
    {
        l.initialize("L" + name, size);
        r.initialize("R" + name, size);
        busName = name;
    }

    void log()
    {
        l.log();
        r.log();
    }

    std::string busName;
    DSPSampleBuffer l;
    DSPSampleBuffer r;
};

class DSPModulationBus
{
public:
    void initialize(const std::string &name, size_t size)
    {
        m.initialize(name, size);
        busName = name;
    }

    void log()
    {
        m.log();
    }

    std::string busName;
    DSPSampleBuffer m;
};

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
     * @brief Registers a new audio channel with the given name.
     *
     * Ensures uniqueness of channel names. If a channel with the same name
     * already exists, the registration is silently ignored.
     * Throws if the maximum number of audio channels is exceeded.
     *
     * @param name The unique name of the audio channel to register.
     * @throws std::runtime_error if the channel limit is reached.
     */
    static DSPAudioBus *registerAudioBus(const std::string &name);

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
    static DSPAudioBus *registerAudioBus(const std::string &name, host_float *outL, host_float *outR);

    /**
     * @brief Registers a new modulation channel with the given name.
     *
     * Ensures that each modulation bus name is unique. If the name already exists,
     * the registration is skipped. Throws if the maximum number of modulation channels is reached.
     *
     * @param name The unique name of the modulation channel to register.
     * @throws std::runtime_error if the modulation channel limit is exceeded.
     */
    static DSPModulationBus *registerModulationBus(const std::string &name);

    /**
     * @brief Returns a pointer to the audio bus with the given name.
     * Returns nullptr if not found.
     *
     * @param name The name of the audio bus.
     * @return Pointer to DSPAudioBus or nullptr.
     */
    static DSPAudioBus *getAudioBus(const std::string &name);

    /**
     * @brief Returns a pointer to the modulation bus with the given name.
     * Returns nullptr if not found.
     *
     * @param name The name of the modulation bus.
     * @return Pointer to DSPModulationBus or nullptr.
     */
    static DSPModulationBus *getModulationBus(const std::string &name);

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

private:
    static constexpr int maxBusses = 128; ///< Maximum number of buses per type

    // Audio bus storage
    static DSPAudioBus audioBusses[maxBusses]; ///< Statically allocated audio bus array
    static std::string audioNames[maxBusses];  ///< Registered names for audio buses
    static int audioCount;                     ///< Current number of registered audio buses

    // Modulation bus storage
    static DSPModulationBus modulationBusses[maxBusses]; ///< Statically allocated modulation bus array
    static std::string modulationNames[maxBusses];       ///< Registered names for modulation buses
    static int modulationCount;                          ///< Current number of registered modulation buses
};
