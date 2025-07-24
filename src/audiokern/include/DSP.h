#pragma once

#include "dsp_types.h"
#include <stddef.h>
#include <string>
#include <mutex>
#include <vector>
#include <iomanip>
#include <sstream>

class DSPObject; // Forward declaration

class DSP
{
public:
    // Logger function for audio host
    using LogFunc = void (*)(const std::string &);

    // Ctor
    DSP();

    // Dtor
    virtual ~DSP();

    // Indicates that the host turned the DSP off
    void off();

    // Initializes the DSP with samplerate and blocksize
    static void initializeAudio(dsp_float rate, size_t size);

    // Must be called when all DSP objects have bin initialized
    static void finalizeAudio();

    // Log function callback registration
    static void registerLogger(LogFunc func);

    // Registers an DSP Object
    static void registerObject(DSPObject &obj);

    // Returns the registry
    static const std::vector<DSPObject*>& getRegistry();

    // Zeros a value if it is in the range +/- epsilon
    static dsp_float zeroSubnormals(dsp_float value);

    // Indicator if the DSP system has been initialized.
    static bool isInitialized() { return initialized; };

    // For various purposes
    static void nextBlock();

    // The max block size
    static constexpr size_t maxBlockSize = 2048;

    // The max sample rate
    static constexpr dsp_float maxSamplerate = 96000.0;

    // Threshold for zeroing
    static constexpr dsp_float epsilon = 1e-10;

    // The audio systems current sampling rate
    static dsp_float sampleRate;

    // The audio systems current sample block size
    static size_t blockSize;

    // Log function
    static void log(const char *fmt, ...);

    // Logs function for buffers
    static void logBuffer(const std::string &label, host_float *buffer, size_t size);

private:
    // Indicator if DSP has been initialized
    static bool initialized;

    // Statistics
    static long elapsedSamples;
    static long processedBlocks;

    static LogFunc logger;

    static std::vector<DSPObject*>& getMutableRegistry();
};