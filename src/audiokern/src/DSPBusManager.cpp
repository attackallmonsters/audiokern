#include "DSPBusManager.h"

DSPObjectCollection<DSPAudioBus> DSPBusManager::audioBusses;
DSPObjectCollection<DSPModulationBus> DSPBusManager::modulationBusses;

void DSPBusManager::initialize()
{
    registerAudioBus(nullBusName);
    registerModulationBus(nullBusName);
}

// ----------- Register Methods -----------

DSPAudioBus &DSPBusManager::registerAudioBus(const std::string &name)
{
    // Skip registration if channel name already exists
    for (size_t i = 0; i < audioBusses.size(); ++i)
    {
        if (audioBusses[i].getName() == name)
            PANIC("DSPBusManager: audio buffer " << audioBusses[i].getName() << " already exists");
    }

    DSPAudioBus *newBus = new DSPAudioBus();
    newBus->initialize(name, DSP::blockSize, true);
    return audioBusses.add(newBus);
}

DSPAudioBus &DSPBusManager::registerAudioBus(const std::string &name, host_float *outL, host_float *outR)
{
    DSPAudioBus &bus = registerAudioBus(name);

    bus.l.assign("L_" + name, outL);
    bus.r.assign("R_" + name, outR);

    return bus;
}

DSPModulationBus &DSPBusManager::registerModulationBus(const std::string &name)
{
    // Skip registration if channel name already exists
    for (size_t i = 0; i < modulationBusses.size(); ++i)
    {
        if (modulationBusses[i].getName() == name)
            PANIC("DSPBusManager: modulation buffer " << modulationBusses[i].getName() << " already exists");
    }

    DSPModulationBus *newBus = new DSPModulationBus();
    newBus->initialize(name, DSP::blockSize, true);
    return modulationBusses.add(newBus);
}

DSPModulationBus &DSPBusManager::registerModulationBus(const std::string &name, host_float *out)
{
    DSPModulationBus &bus = registerModulationBus(name);

    bus.m.assign("L_" + name, out);

    return bus;
}

// ----------- Get Methods -----------

DSPAudioBus &DSPBusManager::getAudioBus(const std::string &name)
{
    for (size_t i = 0; i < audioBusses.size(); ++i)
    {
        if (audioBusses[i].getName() == name)
            return audioBusses[i];
    }

    PANIC("invalid audio bus name: " << name);
}

DSPModulationBus &DSPBusManager::getModulationBus(const std::string &name)
{
    for (size_t i = 0; i < modulationBusses.size(); ++i)
    {
        if (modulationBusses[i].getName() == name)
            return modulationBusses[i];
    }

    PANIC("invalid modulation bus name: " << name);
}

// ----------- Clear -----------

void DSPBusManager::clear()
{
    audioBusses.clear();
    modulationBusses.clear();
}

void DSPBusManager::validate()
{
    for (size_t i = 0; i < audioBusses.size(); ++i)
    {
        try
        {
            audioBusses[i].l.isValid();
            audioBusses[i].r.isValid();
        }
        catch (const std::runtime_error &e)
        {
            DSP::log("AudioBus '%s' validation failed: %s", audioBusses[i].getName().c_str(), e.what());
            throw;
        }
    }

    for (size_t i = 0; i < modulationBusses.size(); ++i)
    {
        try
        {
            modulationBusses[i].m.isValid();
        }
        catch (const std::runtime_error &e)
        {
            DSP::log("ModulationBus '%s' validation failed: %s", modulationBusses[i].getName().c_str(), e.what());
            throw;
        }
    }
}

void DSPBusManager::log()
{
    for (size_t i = 0; i < audioBusses.size(); ++i)
    {
        DSPAudioBus &bus = audioBusses[i];

        DSP::log("Audio bus (managed): %s", bus.getName().c_str());

        DSP::log("  L: name=%s, size=%zu, peak=%.5f",
                 bus.l.getName().c_str(),
                 bus.l.size(),
                 bus.l.peak());

        DSP::log("  R: name=%s, size=%zu, peak=%.5f",
                 bus.r.getName().c_str(),
                 bus.r.size(),
                 bus.r.peak());
    }

    for (size_t i = 0; i < modulationBusses.size(); ++i)
    {
        DSPModulationBus &bus = modulationBusses[i];

        DSP::log("Modulation bus (managed): %s", bus.getName().c_str());

        DSP::log("  L: name=%s, size=%zu, peak=%.5f",
                 bus.m.getName().c_str(),
                 bus.m.size(),
                 bus.m.peak());
    }

    DSPAudioBus::log();
    DSPModulationBus::log();
}
