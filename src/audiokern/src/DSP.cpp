#include <cstring>
#include <cstdarg>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cmath>
#include "clamp.h"
#include "DSP.h"
#include "DSPObject.h"
#include "dsp_types.h"
#include "dsp_math.h"
#include "DSPBusManager.h"

size_t DSP::blockSize = 64;
dsp_float DSP::sampleRate = -1.0;
bool DSP::initialized = false;
long DSP::elapsedSamples = 0;
long DSP::processedBlocks = 0;

// Dummy logger, does nothing
static void defaultLogger(const std::string &)
{
}

DSP::LogFunc DSP::logger = &defaultLogger;

// Contructor
DSP::DSP()
{
    dsp_math::init_trig_lut();
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

    getMutableRegistry().clear();
    DSPBusManager::clear();

    initialized = true;
}

void DSP::finalizeAudio()
{
    for (DSPObject *obj : getRegistry())
    {
        obj->finalize();
    }
}

// Log function callback registration
void DSP::registerLogger(LogFunc func)
{
    logger = func;
}

void DSP::registerObject(DSPObject& obj)
{
    auto& registry = getMutableRegistry();

    // Check for duplicate object name
    for (const auto* existing : registry)
    {
        if (existing->getName() == obj.getName())
        {
            PANIC("DSP::registerObject: object name '" + obj.getName() + "' is already registered");
        }
    }

    // Add new object to the registry
    registry.push_back(&obj);
}

std::vector<DSPObject *> &DSP::getMutableRegistry()
{
    static std::vector<DSPObject *> registry;
    return registry;
}

const std::vector<DSPObject*>& DSP::getRegistry()
{
    return getMutableRegistry();
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

void DSP::logBuffer(const std::string &label, host_float *buffer, size_t size)
{
    constexpr size_t BUFLEN = 8192;
    char msg[BUFLEN];
    size_t pos = 0;

    pos += snprintf(msg + pos, BUFLEN - pos, "%s (%zu): [", label.c_str(), size);

    for (size_t i = 0; i < size; ++i)
    {
        int written = snprintf(msg + pos, BUFLEN - pos, "%.5f%s", buffer[i], (i < size - 1) ? ", " : "]");
        if (written < 0 || static_cast<size_t>(written) >= BUFLEN - pos)
            break;
        pos += static_cast<size_t>(written);
    }

    logger(std::string(msg));
}
