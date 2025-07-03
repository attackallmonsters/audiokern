#include "SineOscillator.h"

SineOscillator::SineOscillator()
{
    // to avoid vtable lookup
    registerSampleGenerator(&SineOscillator::generateSample);
}

void SineOscillator::generateSample(
    Oscillator * /*osc*/,
    const dsp_float & /*frequency*/,
    const dsp_float &phase,
    dsp_float &left,
    dsp_float &right,
    const dsp_float & /*modLeft*/,
    const dsp_float & /*modRight*/)
{
    left = right = std::sin(phase * 2.0 * M_PI);
}