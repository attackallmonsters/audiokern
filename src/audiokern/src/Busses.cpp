#include "DSP.h"
#include "Busses.h"
#include "dsp_math.h"
#include <stdexcept>

DSPBus::DSPBus() : busName(""), isManaged(false)
{
}

DSPBus::~DSPBus()
{
}

// *******************************************
//                DSPBus
// *******************************************

void DSPBus::initialize(const std::string &name, size_t size, bool managed)
{
    busName = name;
    isManaged = managed;

    initializeBus(size);
}

const std::string &DSPBus::getName() const
{
    return busName;
}

bool DSPBus::getIsManaged() const
{
    return isManaged;
}

// *******************************************
//            DSPModulationBus
// *******************************************

DSPObjectCollection<DSPModulationBus> DSPModulationBus::modulationBusses;

void DSPModulationBus::initializeBus(size_t size)
{
    m.initialize(getName(), size);
}

void DSPModulationBus::multiplyWidth(DSPModulationBus &bus)
{
    m.multiplyWith(bus.m);
}

void DSPModulationBus::fill(host_float v)
{
    m.fill(v);
}

void DSPModulationBus::log()
{
    for (size_t i = 0; i < modulationBusses.size(); ++i)
    {
        DSPModulationBus &bus = modulationBusses[i];

        DSP::log("Modulation bus (unmanaged): %s", bus.getName().c_str());

        DSP::log("  L: name=%s, size=%zu, peak=%.5f",
                 bus.m.getName().c_str(),
                 bus.m.size(),
                 bus.m.peak());
    }
}

DSPModulationBus &DSPModulationBus::create(const std::string &name, size_t size)
{
    DSPModulationBus *newBus = new DSPModulationBus();
    newBus->initialize(name, size, false);
    return modulationBusses.add(newBus);
}

// *******************************************
//              DSPAudioBus
// *******************************************

DSPObjectCollection<DSPAudioBus> DSPAudioBus::audioBusses;

void DSPAudioBus::initializeBus(size_t size)
{
    l.initialize("L_" + getName(), size);
    r.initialize("R_" + getName(), size);
}

void DSPAudioBus::multiplyWidth(DSPModulationBus &bus)
{
    l.multiplyWith(bus.m);
    r.multiplyWith(bus.m);
}

void DSPAudioBus::log()
{
    for (size_t i = 0; i < audioBusses.size(); ++i)
    {
        DSPAudioBus &bus = audioBusses[i];

        DSP::log("Audio bus (unmanaged): %s", bus.getName().c_str());

        DSP::log("  L: name=%s, size=%zu, peak=%.5f",
                 bus.l.getName().c_str(),
                 bus.l.size(),
                 bus.l.peak());

        DSP::log("  R: name=%s, size=%zu, peak=%.5f",
                 bus.r.getName().c_str(),
                 bus.r.size(),
                 bus.r.peak());
    }
}

DSPAudioBus &DSPAudioBus::create(const std::string &name, size_t size)
{
    DSPAudioBus *newBus = new DSPAudioBus();
    newBus->initialize(name, size, false);
    return audioBusses.add(newBus);
}