#include "ButterworthFilter.h"

// Constant for 2π
static constexpr host_float TWO_PI = 2.0 * M_PI;

// Constructor
ButterworthFilter::ButterworthFilter()
{
    registerBlockProcessor(processBlock);
}

// Initialize buffers and reset state
void ButterworthFilter::initialize()
{
    bufferL.create(DSP::blockSize);
    bufferR.create(DSP::blockSize);
    reset();
}

void ButterworthFilter::setCutoffFrequency(dsp_float freq)
{
    cutoffFrequency = clamp(freq, 0.0, 20000.0);
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
    filterMode = mode;
}

// Processing function for one block
void ButterworthFilter::processBlock(DSPObject *dsp)
{
    auto *self = static_cast<ButterworthFilter *>(dsp);

    const host_float fc = clamp(self->cutoffFrequency, 5.0, DSP::sampleRate * 0.49);
    const host_float omega = dsp_math::M_2PI * fc / DSP::sampleRate;

    dsp_float sin_omega, cos_omega;
    dsp_math::get_sin_cos(omega, &sin_omega, &cos_omega);

    const host_float alpha = sin_omega / (2.0 * dsp_math::M_1D_SQRT2); // Q = 1/√2

    // Biquad coefficients
    host_float b0, b1, b2, a0, a1, a2;

    if (self->filterMode == FilterMode::LP)
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
        // --- Left channel ---
        const host_float inL = self->bufferL[i];
        const host_float outL = b0 * inL + b1 * self->z1L + b2 * self->z2L - a1 * self->z1L - a2 * self->z2L;

        self->z2L = self->z1L;
        self->z1L = inL;
        self->bufferL[i] = outL;

        // --- Right channel ---
        const host_float inR = self->bufferR[i];
        const host_float outR = b0 * inR + b1 * self->z1R + b2 * self->z2R - a1 * self->z1R - a2 * self->z2R;

        self->z2R = self->z1R;
        self->z1R = inR;
        self->bufferR[i] = outR;
    }
}
