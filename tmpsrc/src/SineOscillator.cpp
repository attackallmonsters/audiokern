#include "SineOscillator.h"

SineOscillator::SineOscillator()
{
    // to avoid vtable lookup
    registerSampleGenerator(&SineOscillator::generateSample);
}

void SineOscillator::generateSample(
    Oscillator * /*osc*/,
    const host_float & /*frequency*/,
    const host_float &phase,
    host_float &left,
    host_float &right,
    const host_float & /*modLeft*/,
    const host_float & /*modRight*/)
{
    left = right = std::sin(phase * 2.0 * dsp_math::DSP_PI);
}