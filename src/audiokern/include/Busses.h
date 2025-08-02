#pragma once

#include "dsp_types.h"
#include "DSPSampleBuffer.h"
#include "DSPObjectCollection.h"
#include <string>

/**
 * @brief Base class for all DSP bus types providing common bus management functionality
 *
 * Provides the fundamental infrastructure for DSP signal routing buses including
 * initialization, lifecycle management, and basic operations. This base class
 * handles common bus properties like naming, management flags, and size tracking
 * while allowing derived classes to implement specific buffer types and operations.
 *
 * Usage:
 * - Derive from this class to create specific bus types (audio, modulation, etc.)
 * - Call initialize() in derived class constructors to set up basic properties
 * - Override pure virtual methods to implement bus-specific functionality
 * - Use isInitialized() to check bus state before operations
 *
 * The base class ensures consistent behavior across all bus types in the DSP
 * infrastructure, providing standardized initialization, naming conventions,
 * and lifecycle management. Derived classes focus on implementing their
 * specific buffer types and processing operations.
 *
 * Example:
 * @code
 * class MyCustomBus : public DSPBusBase
 * {
 * public:
 *     void initialize(const std::string &name, size_t size, bool managed = false) override
 *     {
 *         DSPBusBase::initialize(name, size, managed);
 *         // Initialize custom buffers here
 *     }
 * };
 * @endcode
 */
class DSPBus
{
public:
    /**
     * @brief Default constructor
     *
     * Initializes the base bus object with default values. Bus remains
     * uninitialized until initialize() is called with proper parameters.
     */
    DSPBus();

    /**
     * @brief Virtual destructor for proper cleanup of derived classes
     *
     * Ensures that derived class destructors are called correctly when
     * deleting through base class pointers. Handles cleanup of base
     * class resources and managed bus deregistration if applicable.
     */
    virtual ~DSPBus();

    /**
     * @brief Initialize the bus with basic properties
     *
     * Sets up the fundamental bus properties including name, size, and
     * management flags. This method should be called by derived classes
     * as part of their initialization process to ensure consistent
     * base functionality.
     *
     * @param name Unique identifier for the bus
     * @param size Buffer size, typically DSP::blockSize
     * @param managed True if bus lifecycle is managed automatically
     */
    void initialize(const std::string &name, size_t size, bool managed = false);

    /**
     * @brief Get the current bus name
     *
     * Returns the unique identifier assigned to this bus during initialization.
     * Useful for debugging, logging, and bus lookup operations.
     *
     * @return Const reference to the bus name string
     */
    const std::string &getName() const;

    /**
     * @brief Check if the bus is automatically managed
     *
     * Returns whether this bus is managed by the DSP infrastructure
     * (automatic cleanup) or requires manual lifecycle management.
     *
     * @return True if bus is automatically managed, false otherwise
     */
    bool getIsManaged() const;

    /**
     * @brief Check if the bus has been properly initialized
     *
     * Verifies that initialize() has been called and the bus is ready
     * for use. Should be checked before performing bus operations.
     *
     * @return True if bus is initialized and ready for use
     */
    bool isInitialized() const;

protected:
    /**
     * @brief Initialize the bus with basic properties on derived classes
     *
     * @param size Buffer size, typically DSP::blockSize
     */
    virtual void initializeBus(size_t size) = 0;

private:
    /// @brief Unique name identifier for the bus
    std::string busName;

    /// @brief Flag indicating if bus lifecycle is automatically managed
    bool isManaged;
};

/**
 * @brief DSP modulation bus for control signal routing and processing
 *
 * Manages a single-channel modulation buffer that can be used to control
 * parameters across DSP objects. Modulation buses provide a centralized
 * way to distribute control signals like LFOs, envelopes, or parameter
 * automation throughout the DSP graph.
 *
 * Usage:
 * - Call initialize() to set up the bus with a name and buffer size
 * - Use fill() to populate the buffer with a constant value
 * - Use multiplyWidth() to apply modulation from another bus
 * - Connect DSP objects using connectToModulationBus("busName")
 *
 * Modulation buses are typically created by modulation sources (LFOs,
 * envelopes) and consumed by DSP objects that need parameter control.
 * The managed flag indicates whether the bus lifecycle is handled
 * automatically by the DSP infrastructure.
 *
 * Example:
 * @code
 * DSPModulationBus lfoModBus;
 * lfoModBus.initialize("lfo_mod", DSP::blockSize, true);
 * lfoModBus.fill(0.5f); // Initialize with mid.mange value
 * @endcode
 */
class DSPModulationBus : public DSPBus
{
public:
    /**
     * @brief Apply modulation from another bus by multiplication
     *
     * Multiplies each sample in this bus's buffer with the corresponding
     * sample from the source modulation bus. This allows for modulation
     * of modulation (e.g., tremolo applied to vibrato depth).
     *
     * @param bus Source modulation bus to multiply with
     */
    void multiplyWidth(DSPModulationBus &bus);

    /**
     * @brief Fill the entire modulation buffer with a constant value
     *
     * Sets all samples in the modulation buffer to the specified value.
     * Useful for initializing or setting static parameter values.
     *
     * @param v Value to fill the buffer with (typically 0.0 to 1.0 range)
     */
    void fill(host_float v);

    /**
     * @brief Log bus information for debugging purposes
     *
     * Outputs bus name, management status, and buffer contents to the
     * debug console for troubleshooting modulation routing issues.
     */
    static void log();

    /**
     * @brief Creates a new DSP modulation bus instance
     *
     * Factory method that creates a new DSPModulationBus, initializes it with the
     * specified size. This is the preferred way to create a bus
     * without tracking by DSPBusManager.
     *
     * @param size Buffer size in samples, typically DSP::blockSize
     * @return Reference to the newly created and initialized modulation bus
     */
    static DSPModulationBus &create(const std::string &name, size_t size);

    /// @brief The buffer that holds the modulation data
    DSPSampleBuffer m;

protected:
    /**
     * @brief Initialize the modulation bus with name and buffer size
     *
     * Sets up the internal modulation buffer
     *
     * @param size Buffer size, typically DSP::blockSize
     */
    void initializeBus(size_t size) override;

private:
    /// @brief Just holds the instances created by create(size_t size)
    static DSPObjectCollection<DSPModulationBus> modulationBusses;
};

/**
 * @brief DSP audio bus for stereo audio signal routing and processing
 *
 * Manages stereo (L/R) audio buffers that form the backbone of the DSP
 * signal routing infrastructure. Audio buses allow DSP objects to share
 * audio data efficiently without copying, enabling parallel processing
 * and flexible signal routing.
 *
 * Usage:
 * - Call initialize() to set up the bus with a name and buffer size
 * - Access samples directly via l[n] and r[n] for left/right channels
 * - Use multiplyWidth() to apply modulation to the audio signal
 * - Connect DSP objects using connectToInputBus() or connectToOutputBus()
 *
 * Audio buses can be used for various purposes: input/output routing,
 * effects sends/returns, or internal signal paths. The managed flag
 * indicates whether the bus is automatically handled by the DSP
 * infrastructure or requires manual lifecycle management.
 *
 * Example:
 * @code
 * DSPAudioBus mainOutput;
 * mainOutput.initialize("main_out", DSP::blockSize, true);
 * // Process audio: mainOutput.l[i] = processedSample;
 * @endcode
 */
class DSPAudioBus : public DSPBus
{
public:
    /**
     * @brief Apply modulation to the audio signal
     *
     * Multiplies each audio sample (both L and R channels) with the
     * corresponding modulation value from the source bus. This enables
     * amplitude modulation, tremolo effects, or envelope shaping.
     *
     * @param bus Source modulation bus containing control values
     */
    void multiplyWidth(DSPModulationBus &bus);

    /**
     * @brief Log bus information for debugging purposes
     *
     * Outputs bus name, management status, and buffer contents to the
     * debug console for troubleshooting audio routing and signal flow.
     */
    static void log();

    /**
     * @brief Creates a new DSP audio bus instance
     *
     * Factory method that creates a new DSPAudioBus, initializes it with the
     * specified size. This is the preferred way to create a bus
     * without tracking by DSPBusManager.
     *
     * @param size Buffer size in samples, typically DSP::blockSize
     * @return Reference to the newly created and initialized audio bus
     */
    static DSPAudioBus &create(const std::string &name, size_t size);

    /// @brief The buffer that holds left channel audio samples
    DSPSampleBuffer l;

    /// @brief The buffer that holds right channel audio samples
    DSPSampleBuffer r;

protected:
    /**
     * @brief Initialize the audio bus with name and buffer size
     *
     * Sets up the internal audio buffer
     *
     * @param size Buffer size, typically DSP::blockSize
     */
    void initializeBus(size_t size) override;

private:
    /// @brief Just holds the instances created by create(size_t size)
    static DSPObjectCollection<DSPAudioBus> audioBusses;
};