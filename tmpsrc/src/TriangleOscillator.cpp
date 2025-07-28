#include "TriangleOscillator.h"

TriangleOscillator::TriangleOscillator()
{
    // to avoid vtable lookup
    registerSampleGenerator(&TriangleOscillator::generateSample);
}

void TriangleOscillator::generateSample(
    Oscillator * /*osc*/,
    const host_float & /*frequency*/,
    const host_float &phase,
    host_float &left,
    host_float &right,
    const host_float & /*modLeft*/,
    const host_float & /*modRight*/)
{
    left = right = 4.0 * std::abs(phase - 0.5) - 1.0;
}