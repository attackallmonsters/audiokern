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

void DSPBusManager::registerAudioChannel(const std::string& name)
{
    for (int i = 0; i < audioCount; ++i)
    {
        if (audioNames[i] == name)
            return;
    }

    if (audioCount < maxBusses)
    {
        audioNames[audioCount] = name;
        ++audioCount;
    }
    else
    {
        throw std::runtime_error("Maximum number of audio channels exceeded")
    }
}

void DSPBusManager::registerModulationChannel(const std::string& name)
{
    for (int i = 0; i < modulationCount; ++i)
    {
        if (modulationNames[i] == name)
            return;
    }

    if (modulationCount < maxBusses)
    {
        modulationNames[modulationCount] = name;
        ++modulationCount;
    }
    else
    {
        throw std::runtime_error("Maximum number of modulation channels exceeded")
    }
}

// ----------- Get Methods -----------

DSPAudioBus* DSPBusManager::getAudioBus(const std::string& name)
{
    for (int i = 0; i < audioCount; ++i)
    {
        if (audioNames[i] == name)
            return &audioBusses[i];
    }
    
    return nullptr;
}

DSPModulationBus* DSPBusManager::getModulationBus(const std::string& name)
{
    for (int i = 0; i < modulationCount; ++i)
    {
        if (modulationNames[i] == name)
            return &modulationBusses[i];
    }

    return nullptr;
}

// ----------- Initialization -----------

void DSPBusManager::initialize(size_t bufferSize)
{
    for (int i = 0; i < audioCount; ++i)
        audioBusses[i].initialize(audioNames[i], bufferSize);

    for (int i = 0; i < modulationCount; ++i)
        modulationBusses[i].initialize(modulationNames[i], bufferSize);

    for (int i = 0; i < signalCount; ++i)
        signalBusses[i].initialize(signalNames[i], bufferSize);
}

// ----------- Clear -----------

void DSPBusManager::clear()
{
    audioCount = 0;
    modulationCount = 0;
    signalCount = 0;
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
        catch (const std::runtime_error& e)
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
        catch (const std::runtime_error& e)
        {
            DSP::log("ModulationBus '%s' validation failed: %s", modulationNames[i].c_str(), e.what());
            throw;
        }
    }

    for (int i = 0; i < signalCount; ++i)
    {
        try
        {
            signalBusses[i].m.isValid();
        }
        catch (const std::runtime_error& e)
        {
            DSP::log("SignalBus '%s' validation failed: %s", signalNames[i].c_str(), e.what());
            throw;
        }
    }
}
