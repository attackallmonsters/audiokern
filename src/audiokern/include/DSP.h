#pragma once

#include "dsp_types.h"
#include <stddef.h>
#include <string>
#include <mutex>
#include <vector>
#include <iomanip>
#include <sstream>

class DSPObject; // Forward declaration

/**
 * @brief Static utility and management class for the DSP system.
 * 
 * Provides centralized initialization, logging, and global object registration
 * for all DSP-related components.
 */
class DSP
{
public:
    /// Function pointer type for logging callback
    using LogFunc = void (*)(const std::string&);

    /// Constructs a DSP instance (currently unused in static context)
    DSP();

    /// Virtual destructor
    virtual ~DSP();

    /**
     * @brief Called by the host to signal DSP shutdown.
     * 
     * Can be used to stop processing or release audio resources.
     */
    static void off();

    /**
     * @brief Initializes the DSP system with sampling rate and block size.
     * 
     * This must be called before using any DSP objects or processing buffers.
     * 
     * @param rate  Sampling rate in Hz (e.g. 44100.0, 48000.0)
     * @param size  Block size in samples (must be > 0 and <= maxBlockSize)
     */
    static void initializeAudio(dsp_float rate, size_t size);

    /**
     * @brief Finalizes DSP setup after all objects have been created and connected to busses.
     * 
     * Typically called once at the end of system initialization to prepare
     * connections, precompute tables, or verify integrity.
     */
    static void finalizeAudio();

    /**
     * @brief Registers a log callback function for textual DSP output.
     * 
     * The callback is invoked via DSP::log(...) and must be thread-safe.
     * 
     * @param func A function accepting a std::string log message.
     */
    static void registerLogger(LogFunc func);

    /**
     * @brief Registers a DSPObject with the global registry.
     * 
     * Each DSPObject must have a unique name. Duplicate names will throw.
     * 
     * @param obj Reference to the DSP object to register.
     * @throws std::runtime_error if an object with the same name already exists.
     */
    static void registerObject(DSPObject& obj);

    /**
     * @brief Returns the list of all registered DSP objects.
     * 
     * Used for traversal, debugging, or system-wide messaging.
     * 
     * @return Const reference to internal object registry.
     */
    static const std::vector<DSPObject*>& getRegistry();

    /**
     * @brief Replaces very small values with zero to avoid denormals.
     * 
     * This avoids costly floating-point denormal behavior on some CPUs.
     * 
     * @param value The sample value to check.
     * @return Zero if abs(value) < epsilon, otherwise value.
     */
    static dsp_float zeroSubnormals(dsp_float value);

    /**
     * @brief Indicates whether the DSP system has been successfully initialized.
     * 
     * @return True if initializeAudio(...) has been called.
     */
    static bool isInitialized() { return initialized; }

    /**
     * @brief Advances internal block statistics (e.g., block count).
     * 
     * Should be called once per processed audio block.
     */
    static void nextBlock();

    /// Maximum allowed sample block size
    static constexpr size_t maxBlockSize = 2048;

    /// Maximum supported sampling rate
    static constexpr dsp_float maxSamplerate = 96000.0;

    /// Threshold for denormal suppression
    static constexpr dsp_float epsilon = 1e-10;

    /// Current system sampling rate in Hz
    static dsp_float sampleRate;

    /// Current block size in samples
    static size_t blockSize;

    /**
     * @brief Formatted log message output (like printf).
     * 
     * Uses the registered log function, if available.
     * 
     * @param fmt printf-style format string
     * @param ... optional format arguments
     */
    static void log(const char* fmt, ...);

    /**
     * @brief Formatted log message output (like printf).
     * 
     * Uses the registered log function, if available.
     * 
     * @param interval DSP loop interval (1000/samplerate (44100) * blocksize (64) = 1 interval (1.45ms))
     * @param fmt printf-style format string
     * @param ... optional format arguments
     */
    void log(size_t interval, const char *fmt, ...);

    /**
     * @brief Logs the contents of a buffer with label.
     * 
     * Outputs each value to the log function in formatted form.
     * 
     * @param label Descriptive name or tag for the buffer.
     * @param buffer Pointer to the buffer to log.
     * @param size Number of elements in the buffer.
     */
    static void logBuffer(const std::string& label, host_float* buffer, size_t size);

private:
    /// log function
    static void logv(const char* fmt, va_list args);

    /// Internal flag: true after successful initialization
    static bool initialized;

    /// Total number of processed samples (for stats)
    static long elapsedSamples;

    /// Total number of processed blocks (for stats)
    static long processedBlocks;

    /// Current logger callback function
    static LogFunc logger;

    /// Current log intervall counter
    static size_t currentLogInterval;

    /**
     * @brief Returns the mutable internal DSP object registry.
     * 
     * Used by registerObject(...) to store object pointers.
     */
    static std::vector<DSPObject*>& getMutableRegistry();
};
