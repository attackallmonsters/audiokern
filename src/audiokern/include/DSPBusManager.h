
#pragma once

#include "DSPSampleBuffer.h"
#include <stdexcept>

struct StandardBus
{
    static const std::string Output;
    static const std::string Input;
    static const std::string Wet;
    static const std::string FM;
};

const std::string StandardBus::Output = "out";
const std::string StandardBus::Input  = "in";
const std::string StandardBus::Wet    = "wet";
const std::string StandardBus::FM     = "fm";

class DSPAudioBus
{
public:
    void initialize(const std::string &name, size_t size)
    {
        l.initialize("L_" + name, size);
        r.initialize("R_" + name, size);
    }

    DSPSampleBuffer l;
    DSPSampleBuffer r;
};

class DSPModulationBus
{
public:
    void initialize(const std::string &name, size_t size)
    {
        m.initialize(name, size);
    }

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
     * @brief Registers a new audio bus with the given name.
     * If the name already exists, the call is ignored.
     * 
     * @param name The unique name of the audio bus.
     */
    static void registerAudioChannel(const std::string& name);

    /**
     * @brief Registers a new modulation bus with the given name.
     * If the name already exists, the call is ignored.
     * 
     * @param name The unique name of the modulation bus.
     */
    static void registerModulationChannel(const std::string& name);

    /**
     * @brief Returns a pointer to the audio bus with the given name.
     * Returns nullptr if not found.
     * 
     * @param name The name of the audio bus.
     * @return Pointer to DSPAudioBus or nullptr.
     */
    static DSPAudioBus* getAudioBus(const std::string& name);

    /**
     * @brief Returns a pointer to the modulation bus with the given name.
     * Returns nullptr if not found.
     * 
     * @param name The name of the modulation bus.
     * @return Pointer to DSPModulationBus or nullptr.
     */
    static DSPModulationBus* getModulationBus(const std::string& name);

    /**
     * @brief Initializes all registered buses with the given buffer size.
     * This must be called after DSP::blockSize is known.
     * 
     * @param bufferSize The block size to allocate for each bus buffer.
     */
    static void initialize(size_t bufferSize);

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

private:
    static constexpr int maxBusses = 32;  ///< Maximum number of buses per type

    // Audio bus storage
    static DSPAudioBus audioBusses[maxBusses];   ///< Statically allocated audio bus array
    static std::string audioNames[maxBusses];    ///< Registered names for audio buses
    static int audioCount;                       ///< Current number of registered audio buses

    // Modulation bus storage
    static DSPModulationBus modulationBusses[maxBusses]; ///< Statically allocated modulation bus array
    static std::string modulationNames[maxBusses];       ///< Registered names for modulation buses
    static int modulationCount;                          ///< Current number of registered modulation buses
};
