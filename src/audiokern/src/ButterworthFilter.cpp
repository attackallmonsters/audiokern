#include "ButterworthFilter.h"

// Constant for 2π
static constexpr host_float TWO_PI = 2.0 * dsp_math::DSP_PI;

// Constructor
ButterworthFilter::ButterworthFilter()
{
    registerBlockProcessor(processBlock);
}

// Initialize buffers and reset state
void ButterworthFilter::initializeProcessor()
{
    reset();
    slew.initialize("slew" + getName());
    slew.setSlewTime(10.0f);
}

void ButterworthFilter::setCutoffFrequency(host_float freq)
{
    cutoffFrequency = clamp(freq, 5.0, DSP::sampleRate * 0.49);
    slew.setTarget(cutoffFrequency);
}

// Reset internal filter states
void ButterworthFilter::reset()
{
    z1L = z2L = 0.0;
    z1R = z2R = 0.0;
}

// Set the filter mode (Lowpass or Highpass)
void ButterworthFilter::setFilterMode(FilterMode mode)
{
    reset();
    filterMode = mode;
}

void ButterworthFilter::processBlock()
{
    const host_float omega = dsp_math::DSP_2PI * slew.processBlock() / DSP::sampleRate;

    host_float sin_omega, cos_omega;
    dsp_math::get_sin_cos(omega, &cos_omega, &sin_omega);

    const host_float alpha = sin_omega / (2.0 * dsp_math::DSP_1D_SQRT2); // Q = 1/√2

    // Biquad coefficients
    host_float b0, b1, b2, a0, a1, a2;

    if (filterMode == FilterMode::LP)
    {
        b0 = (1.0 - cos_omega) / 2.0;
        b1 = 1.0 - cos_omega;
        b2 = (1.0 - cos_omega) / 2.0;
    }
    else // Highpass
    {
        b0 = (1.0 + cos_omega) / 2.0;
        b1 = -(1.0 + cos_omega);
        b2 = (1.0 + cos_omega) / 2.0;
    }

    a0 = 1.0 + alpha;
    a1 = -2.0 * cos_omega;
    a2 = 1.0 - alpha;

    // Normalize coefficients
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Left channel
        const host_float inL = processBus.l[i];
        const host_float outL = b0 * inL + b1 * x1L + b2 * x2L - a1 * y1L - a2 * y2L;

        x2L = x1L;
        x1L = inL;
        y2L = y1L;
        y1L = outL;
        processBus.l[i] = outL;

        // Right channel
        const host_float inR = processBus.r[i];
        const host_float outR = b0 * inR + b1 * x1R + b2 * x2R - a1 * y1R - a2 * y2R;

        x2R = x1R;
        x1R = inR;
        y2R = y1R;
        y1R = outR;
        processBus.r[i] = outR;
    }
}

// Processing function for one block
void ButterworthFilter::processBlock(DSPObject *dsp)
{
    ButterworthFilter * self = static_cast<ButterworthFilter *>(dsp);
    self->processBlock();
}
