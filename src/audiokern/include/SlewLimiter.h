#pragma once

#include "DSP.h"
#include "DSPObject.h"
#include "dsp_types.h"

class SlewLimiter : public DSPObject
{
public:
    // Constructor: slewTimeMs in milliseconds
    SlewLimiter();

    // Set new target (starts smoothing)
    void setTarget(dsp_float newTarget);

    // Sets the slew time in milliseconds
    void setSlewTime(double ms);

    // Advance one sample
    dsp_float process();

    // Restarts the process
    void restart();

    // Slew is idle (has target value)
    bool isIdle() { return idle; }

    // Gets the current slew status
    double get() const { return current; }

protected:
    // Initializes the slew limiter
    DSPUsage initializeObject() override;

private:
    // Calculates the samples for current samplerate
    void calcSamples();

    size_t samplerate;
    size_t slewSamples;
    size_t remaining = 0;
    dsp_float current = 0.0;
    dsp_float target = 0.0;
    dsp_float step = 0.0;
    bool idle;
    dsp_float slewTime;
};
