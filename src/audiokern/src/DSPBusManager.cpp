#include "DSPBusManager.h"

// Audio
DSPAudioBus DSPBusManager::audioBusses[maxBusses];
std::string DSPBusManager::audioNames[maxBusses];
int DSPBusManager::audioCount = 0;

// Modulation
DSPModulationBus DSPBusManager::modulationBusses[maxBusses];
std::string DSPBusManager::modulationNames[maxBusses];
int DSPBusManager::modulationCount = 0;

// ----------- Register Methods -----------

DSPAudioBus *DSPBusManager::registerAudioBus(const std::string &name)
{
    // Skip registration if channel name already exists
    for (int i = 0; i < audioCount; ++i)
    {
        if (audioNames[i] == name)
            PANIC("DSPBusManager: audio buffer " << name << " already exists");
    }

    // Register new channel if space allows
    if (audioCount < maxBusses)
    {
        audioNames[audioCount] = name;
        audioBusses[audioCount].initialize(name, DSP::blockSize);
        ++audioCount;

        return getAudioBus(name);
    }
    else
    {
        PANIC("DSPBusManager: maximum number of audio channels exceeded");
    }
}

DSPAudioBus *DSPBusManager::registerAudioBus(const std::string &name, host_float *outL, host_float *outR)
{
    DSPAudioBus *bus = registerAudioBus(name);

    bus->l.assign("L_" + name, outL);
    bus->r.assign("R_" + name, outR);

    return bus;
}

DSPModulationBus *DSPBusManager::registerModulationBus(const std::string &name)
{
    // Skip registration if channel name already exists
    for (int i = 0; i < modulationCount; ++i)
    {
        if (modulationNames[i] == name)
            PANIC("DSPBusManager: modulation buffer " << name << " already exists");
    }

    // Register new channel if space allows
    if (modulationCount < maxBusses)
    {
        modulationNames[modulationCount] = name;
        modulationBusses[modulationCount].initialize(name, DSP::blockSize);
        ++modulationCount;

        return getModulationBus(name);
    }
    else
    {
        PANIC("DSPBusManager: maximum number of modulation channels exceeded");
    }
}

// ----------- Get Methods -----------

DSPAudioBus *DSPBusManager::getAudioBus(const std::string &name)
{
    for (int i = 0; i < audioCount; ++i)
    {
        if (audioNames[i] == name)
            return &audioBusses[i];
    }

    PANIC("invalid audio bus name: " << name);
}

DSPModulationBus *DSPBusManager::getModulationBus(const std::string &name)
{
    for (int i = 0; i < modulationCount; ++i)
    {
        if (modulationNames[i] == name)
            return &modulationBusses[i];
    }

    PANIC("invalid modulation bus name: " << name);
}

// ----------- Clear -----------

void DSPBusManager::clear()
{
    audioCount = 0;
    modulationCount = 0;

    for (int i; i < maxBusses; ++i)
    {
        audioNames[i] = "";
    }
}

void DSPBusManager::validate()
{
    for (int i = 0; i < audioCount; ++i)
    {
        try
        {
            audioBusses[i].l.isValid();
            audioBusses[i].r.isValid();
        }
        catch (const std::runtime_error &e)
        {
            DSP::log("AudioBus '%s' validation failed: %s", audioNames[i].c_str(), e.what());
            throw;
        }
    }

    for (int i = 0; i < modulationCount; ++i)
    {
        try
        {
            modulationBusses[i].m.isValid();
        }
        catch (const std::runtime_error &e)
        {
            DSP::log("ModulationBus '%s' validation failed: %s", modulationNames[i].c_str(), e.what());
            throw;
        }
    }
}

void DSPBusManager::log()
{
    for (int i = 0; i < audioCount; ++i)
    {
        if (audioNames[i].empty())
            continue;

        DSPAudioBus &bus = audioBusses[i];

        DSP::log("Audio bus: %s", bus.busName.c_str());

        DSP::log("  L: name=%s, size=%zu, peak=%.5f",
                 bus.l.getName().c_str(),
                 bus.l.size(),
                 bus.l.peak());

        DSP::log("  R: name=%s, size=%zu, peak=%.5f",
                 bus.r.getName().c_str(),
                 bus.r.size(),
                 bus.r.peak());
    }

    for (int i = 0; i < modulationCount; ++i)
    {
        if (modulationNames[i].empty())
            continue;

        DSPModulationBus &bus = modulationBusses[i];

        DSP::log("Modulation bus: %s", bus.busName.c_str());

        DSP::log("  L: name=%s, size=%zu, peak=%.5f",
                 bus.m.getName().c_str(),
                 bus.m.size(),
                 bus.m.peak());
    }
}