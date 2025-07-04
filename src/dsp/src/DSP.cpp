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
bool DSP::logFileInitialized = false;
bool DSP::logIsEnabled = true;
int DSP::logSampels = 22050;
long DSP::elapsedSamples = 0;

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

// Initializes the DSP with samplerate and blocksize
void DSP::initializeAudio(dsp_float rate, size_t size)
{
    if (initialized)
        return;

    sampleRate = clamp(rate, 1.0, maxSamplerate);
    blockSize = clamp(size, static_cast<size_t>(1), maxBlockSize);

    DSP::log("DSP audio settings: samplerate is %f", sampleRate);
    DSP::log("DSP audio settings: block size is %i", blockSize);

    logTime(500);

    initialized = true;
}

// Log function callback registration
void DSP::registerLogger(LogFunc func)
{
    logger = func;
    logFileInitialized = false;
}

void DSP::log(const char *fmt, ...)
{
    if (!logIsEnabled)
        return;

    if (++elapsedSamples % logSampels != 0)
    {
        return;
    }

    char buffer[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    logger(std::string(buffer));
}

// Logging to file "dsp.log"
void DSP::log2File(const char *fmt, ...)
{
    if (!logIsEnabled)
        return;

    if (++elapsedSamples % logSampels != 0)
    {
        return;
    }

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

// Enables or disables log globally
void DSP::enableLog(bool isEnabled)
{
    logIsEnabled = isEnabled;
}

// Zeros a value if it is in the range of +/- epsilon
dsp_float DSP::zeroSubnormals(dsp_float value)
{
    return (std::fabs(value) < epsilon) ? 0.0 : value;
}