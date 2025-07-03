#include "SquareOscillator.h"
#include "clamp.h"
#include "dsp_types.h"

SquareOscillator::SquareOscillator()
{
    // to avoid vtable lookup
    registerSampleGenerator(&SquareOscillator::generateSample);
}

void SquareOscillator::generateSample(
    Oscillator *osc,
    const dsp_float & /*frequency*/,
    const dsp_float &phase,
    dsp_float &left,
    dsp_float &right,
    const dsp_float & /*modLeft*/,
    const dsp_float & /*modRight*/)
{
    SquareOscillator *square = static_cast<SquareOscillator *>(osc);

    // Generate square wave: output +1.0 if phase is less than dutyCycle, else -1.0
    left = right = (phase < square->dutyCycle) ? 1.0 : -1.0;
}

// Sets the duty cycle for PWM
void SquareOscillator::setDutyCycle(dsp_float value)
{
    dutyCycle = clamp(value, 0.05, 0.95);
}