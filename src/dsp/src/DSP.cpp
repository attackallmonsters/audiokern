#include <cstring>
#include <cstdarg>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cmath>
#include "clamp.h"
#include "DSP.h"
#include "dsp_types.h"

size_t DSP::blockSize = 64;
dsp_float DSP::sampleRate = -1.0;
bool DSP::initialized = false;
long DSP::elapsedSamples = 0;
long DSP::processedBlocks = 0;

#if DEBUG
bool DSP::logFileInitialized = false;
bool DSP::logIsEnabled = true;
int DSP::logSampels = 22050;
int DSP::logSampleCounter = 0;
bool DSP::resetLogSampleCounter = false;
#endif

// Dummy logger, does nothing
static void defaultLogger(const std::string &)
{
}

DSP::LogFunc DSP::logger = &defaultLogger;

// Contructor
DSP::DSP()
{
}

// Destructor
DSP::~DSP()
{
}

// Indicates that the host turned the DSP off
void DSP::off()
{
    initialized = false;
}

void DSP::nextBlock()
{
    elapsedSamples += blockSize;
    processedBlocks++;

#ifdef DEBUG
    if (resetLogSampleCounter)
    {
        logSampleCounter = 0;
        resetLogSampleCounter = false;
    }

    logSampleCounter += blockSize;
#endif
}

// Initializes the DSP with samplerate and blocksize
void DSP::initializeAudio(dsp_float rate, size_t size)
{
    if (initialized)
        return;

    sampleRate = clamp(rate, 1.0, maxSamplerate);
    blockSize = clamp(size, static_cast<size_t>(1), maxBlockSize);

    DSP::log("DSP audio settings: samplerate is %f", sampleRate);
    DSP::log("DSP audio settings: block size is %i", blockSize);
#ifdef DEBUG
    DSP::logTime(300);
#endif

    initialized = true;
}

// Log function callback registration
void DSP::registerLogger(LogFunc func)
{
    logger = func;

#if DEBUG
    logFileInitialized = false;
#endif
}

// Zeros a value if it is in the range of +/- epsilon
dsp_float DSP::zeroSubnormals(dsp_float value)
{
    return (std::fabs(value) < epsilon) ? 0.0 : value;
}

void DSP::log(const char *fmt, ...)
{
    char buffer[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    logger(std::string(buffer));
}

#ifdef DEBUG
void DSP::dspLog(const char *fmt, ...)
{
    if (!doLog())
        return;
        
    char buffer[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    logger(std::string(buffer));
}

void DSP::dspLogBuffer(const std::string &label, const DSPSampleBuffer &buffer)
{
    if (!doLog())
        return;

    constexpr size_t MAX_LOG = 32;
    constexpr size_t BUFLEN = 2048;
    char buf[BUFLEN];
    size_t pos = 0;

    pos += snprintf(buf + pos, BUFLEN - pos, "%s [", label.c_str());

    for (size_t i = 0; i < std::min(buffer.size(), size_t(MAX_LOG)); ++i)
    {
        pos += snprintf(buf + pos, BUFLEN - pos, "%.4f%s", buffer[i], (i < MAX_LOG - 1 ? ", " : ""));
        if (pos >= BUFLEN - 16)
            break;
    }

    if (buffer.size() > MAX_LOG)
        pos += snprintf(buf + pos, BUFLEN - pos, ", ...");

    snprintf(buf + pos, BUFLEN - pos, "]");

    logger(std::string(buf));
}

void DSP::dspLogBuffer(const std::string &label, const DSPBuffer &buffer)
{
    if (!doLog())
        return;

    constexpr size_t MAX_LOG = 32;
    constexpr size_t BUFLEN = 2048;
    char buf[BUFLEN];
    size_t pos = 0;

    pos += snprintf(buf + pos, BUFLEN - pos, "%s [", label.c_str());

    for (size_t i = 0; i < std::min(buffer.size(), size_t(MAX_LOG)); ++i)
    {
        pos += snprintf(buf + pos, BUFLEN - pos, "%.4f%s", buffer[i], (i < MAX_LOG - 1 ? ", " : ""));
        if (pos >= BUFLEN - 16)
            break;
    }

    if (buffer.size() > MAX_LOG)
        pos += snprintf(buf + pos, BUFLEN - pos, ", ...");

    snprintf(buf + pos, BUFLEN - pos, "]");

    logger(std::string(buf));
}

void DSP::dspLogBuffer(const std::string &label, host_float *buffer)
{
    if (!doLog())
        return;

    constexpr size_t MAX_LOG = 32;
    constexpr size_t BUFLEN = 2048;
    char buf[BUFLEN];
    size_t pos = 0;

    pos += snprintf(buf + pos, BUFLEN - pos, "%s [", label.c_str());

    for (size_t i = 0; i < std::min(blockSize, size_t(MAX_LOG)); ++i)
    {
        pos += snprintf(buf + pos, BUFLEN - pos, "%.4f%s", buffer[i], (i < MAX_LOG - 1 ? ", " : ""));
        if (pos >= BUFLEN - 16)
            break;
    }

    if (blockSize > MAX_LOG)
        pos += snprintf(buf + pos, BUFLEN - pos, ", ...");

    snprintf(buf + pos, BUFLEN - pos, "]");

    logger(std::string(buf));
}

// Logging to file "dsp.log"
void DSP::dspLog2File(const char *fmt, ...)
{
    if (!doLog())
        return;

    const char *logFileName = "dsp.log";

    const char *mode = logFileInitialized ? "a" : "w";
    FILE *file = std::fopen(logFileName, mode);
    if (!file)
        return;

    logFileInitialized = true;

    // Creates timestamp
    using namespace std::chrono;
    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t t = system_clock::to_time_t(now);

    char timebuf[32];
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::strftime(timebuf, sizeof(timebuf), "%H:%M:%S", &tm);

    char timestamp[64];
    std::snprintf(timestamp, sizeof(timestamp), "[%s.%03d] ", timebuf, static_cast<int>(ms.count()));

    // Formatting
    char msgbuf[2048];
    va_list args;
    va_start(args, fmt);
    std::vsnprintf(msgbuf, sizeof(msgbuf), fmt, args);
    va_end(args);

    // Write and close file
    std::fprintf(file, "%s%s\n", timestamp, msgbuf);
    std::fclose(file);
}

void DSP::logTime(int timeMs)
{
    logSampels = sampleRate / 1000 * timeMs;
}

bool DSP::doLog()
{
    if (logSampleCounter > logSampels)
    {
        resetLogSampleCounter = true;
        return true;
    }

    return false;
}

// Enables or disables log globally
void DSP::enableLog(bool isEnabled)
{
    logIsEnabled = isEnabled;
}
#endif
