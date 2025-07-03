#include "TriangleOscillator.h"
#include <vector>
#include <cmath>
#include "dsp_types.h"

TriangleOscillator::TriangleOscillator()
{
    // to avoid vtable lookup
    registerSampleGenerator(&TriangleOscillator::generateSample);
}

void TriangleOscillator::generateSample(
    Oscillator * /*osc*/,
    const dsp_float & /*frequency*/,
    const dsp_float &phase,
    dsp_float &left,
    dsp_float &right,
    const dsp_float & /*modLeft*/,
    const dsp_float & /*modRight*/)
{
    left = right = 4.0 * std::abs(phase - 0.5) - 1.0;
}