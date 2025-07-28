#include "SawOscillator.h"

SawOscillator::SawOscillator()
{
    // to avoid vtable lookup
    registerSampleGenerator(&SawOscillator::generateSample);
}

void SawOscillator::generateSample(
    Oscillator * /*osc*/,
    const host_float & /*frequency*/,
    const host_float &phase,
    host_float &left,
    host_float &right,
    const host_float & /*modLeft*/,
    const host_float & /*modRight*/)
{
    // Raw sawtooth signal from -1.0 to +1.0
    left = right = 2.0 * phase - 1.0;
}
