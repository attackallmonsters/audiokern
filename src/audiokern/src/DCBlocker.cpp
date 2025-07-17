#include "DCBlocker.h"

DCBlocker::DCBlocker(dsp_float r)
    : x1(0.0), y1(0.0), R(r)
{
}

void DCBlocker::reset()
{
    x1 = 0.0;
    y1 = 0.0;
}

void DCBlocker::setCoefficient(dsp_float r)
{
    R = r;
}

dsp_float DCBlocker::processSample(dsp_float input)
{
    // Apply high-pass filter to remove DC
    double output = input - x1 + R * y1;

    // Update internal state
    x1 = input;
    y1 = output;

    return output;
}
