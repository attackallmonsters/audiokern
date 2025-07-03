// SquareOscillator.h

#pragma once

#include "Oscillator.h"
#include <vector>
#include <cmath>
#include "dsp_types.h"

// Squaretooth oscillator generates square waveform
class SquareOscillator : public Oscillator
{
public:
    // Default constructor
    SquareOscillator();

    // Sets the duty cycle for PWM
    void setDutyCycle(dsp_float value) override;

private:
    // Sample calculation without looking up vtable
    static void generateSample(
        Oscillator *,
        const dsp_float &,
        const dsp_float &,
        dsp_float &,
        dsp_float &,
        const dsp_float &,
        const dsp_float &);
    
    dsp_float dutyCycle = 0.5; // Duty cycle for square oscillator
};