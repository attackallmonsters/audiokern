// SquareOscillator.h

#pragma once

#include "Oscillator.h"
#include "dsp_types.h"
#include "clamp.h"
#include <vector>
#include <cmath>

// Squaretooth oscillator generates square waveform
class SquareOscillator : public Oscillator
{
public:
    // Default constructor
    SquareOscillator();

    // Sets the duty cycle for PWM
    void setDutyCycle(host_float value) override;

private:
    // Sample calculation without looking up vtable
    static void generateSample(
        Oscillator *,
        const host_float &,
        const host_float &,
        host_float &,
        host_float &,
        const host_float &,
        const host_float &);
    
    host_float dutyCycle = 0.5; // Duty cycle for square oscillator
};