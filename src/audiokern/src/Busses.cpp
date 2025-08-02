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

DSPObjectCollection<DSPModulationBus> DSPModulationBus::busPool;

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
    m.log();
}

DSPModulationBus &DSPModulationBus::create(size_t size)
{
    DSPModulationBus *newBus = new DSPModulationBus();
    newBus->initialize(dsp_math::unique_string_id("bus_"), size, false);
    return busPool.add(newBus);
}

// *******************************************
//              DSPAudioBus
// *******************************************

DSPObjectCollection<DSPAudioBus> DSPAudioBus::busPool;

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
    l.log();
    r.log();
}

DSPAudioBus &DSPAudioBus::create(size_t size)
{
    DSPAudioBus *newBus = new DSPAudioBus();
    newBus->initialize(dsp_math::unique_string_id("bus_"), size, false);
    return busPool.add(newBus);
}