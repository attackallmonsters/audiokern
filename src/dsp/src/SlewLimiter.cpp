#include "SlewLimiter.h"

// Constructor: slewTimeMs in milliseconds
SlewLimiter::SlewLimiter(dsp_float ms)
{
    slewTime = ms;
}

// Initializes the slew limiter
void SlewLimiter::initialize()
{
    DSPObject::initialize();
    
    samplerate = DSP::sampleRate;
    remaining = 0;
    current = 0.0;
    target = 0.0;
    step = 0.0;
    calcSamples();
}

// Set new target (starts smoothing)
void SlewLimiter::setTarget(dsp_float newTarget)
{
    target = newTarget;
    idle = false;

    if (slewSamples > 0)
    {
        step = (target - current) / static_cast<dsp_float>(slewSamples);
        remaining = slewSamples;
    }
    else
    {
        current = target;
        step = 0.0;
        remaining = 0;
        idle = true;
    }
}

// Sets the slew time in milliseconds
void SlewLimiter::setSlewTime(double ms)
{
    slewTime = ms;
    calcSamples();
}

// Calculates the samples for current samplerate
void SlewLimiter::calcSamples()
{
    slewSamples = static_cast<size_t>(slewTime * samplerate * 0.001);
}

// Advance one sample
dsp_float SlewLimiter::process()
{
    if (remaining > 0)
    {
        current += step;
        --remaining;
    }
    else
    {
        current = target;
        idle = true;
    }

    return current;
}

// Restarts the process
void SlewLimiter::restart()
{
    setTarget(target);
}