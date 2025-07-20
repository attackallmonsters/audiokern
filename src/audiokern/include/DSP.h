#pragma once

#include "DSPSampleBuffer.h"
#include "DSPBuffer.h"
#include "dsp_types.h"
#include <stddef.h>
#include <string>
#include <mutex>
#include <vector>

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

    // Log function callback registration
    static void registerLogger(LogFunc func);

    // Zeros a value if it is in the range +/- epsilon
    static dsp_float zeroSubnormals(dsp_float value);

    // Indicator if the DSP system has been initialized.
    static bool isInitialized() { return initialized; };

    // Log function
    static void log(const char *fmt, ...);

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

#ifdef DEBUG
    // Audio rate log function, needs DSP::nextBlock() once in the DSP loop
    static void dspLog(const char *fmt, ...);

    // Logs the content of a DSPSampleBuffer at audio rate, needs DSP::nextBlock() once in the DSP loop
    static void dspLogBuffer(const std::string &label, const DSPSampleBuffer &buffer);

    // Logs the content of a DSPBuffer at audio rate, needs DSP::nextBlock() once in the DSP loop
    static void dspLogBuffer(const std::string &label, const DSPBuffer &buffer);

    // Logs the content of a host_float buffer with block size at audio rate, needs DSP::nextBlock() once in the DSP loop
    static void dspLogBuffer(const std::string &label, host_float *buffer);

    // Logs the content of a host_float buffer with block size at audio rate, needs DSP::nextBlock() once in the DSP loop
    static void dspLogBuffer(const std::string &label, std::vector<host_float> *buffer);

    // Logs a buffer to dsp.log
    static void dspLog2File(const char* label, const DSPSampleBuffer& buffer);

    // Logs a buffer to dsp.log
    static void dspLog2File(const char* label, const host_float* data);

    // Logs a buffer to dsp.log
    static void dspLog2File(const char* label, const std::vector<host_float>* vec);

    // Log to file function at audio rate, needs DSP::nextBlock() once in the DSP loop
    static void dspLog2File(const char *fmt, ...);

    // Log output interval
    static void logTime(int timeMs);

    // Enables or disables log globally
    static void enableLog(bool isEnabled);
#endif

private:
    // Indicator if DSP has been initialized
    static bool initialized;

    // Statistics
    static long elapsedSamples;
    static long processedBlocks;

#if DEBUG
    // Logging callback for audio host system
    static bool logFileInitialized;
    static bool logIsEnabled;
    static int logSampels;
    static int logSampleCounter;
    static bool resetLogSampleCounter;

    static bool doLog();
#endif
    static LogFunc logger;
};