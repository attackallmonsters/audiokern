// === KorgonFilter.cpp ===
#include "KorgonFilter.h"

// Constructor with sample rate
KorgonFilter::KorgonFilter()
{
    registerBlockProcessor(&KorgonFilter::processBlockLP);
}

// Initializes the filter
void KorgonFilter::initializeProcessor()
{
    setDrive(0.0);
    reset();

    T = 1.0 / DSP::sampleRate;
}

void KorgonFilter::setFilterMode(FilterMode mode)
{
    switch (mode)
    {
    case FilterMode::LP:
        filterMode = FilterMode::LP;
        registerBlockProcessor(&KorgonFilter::processBlockLP);
        break;
    case FilterMode::HP:
        filterMode = FilterMode::HP;
        registerBlockProcessor(&KorgonFilter::processBlockHP);
        break;
    default:
        filterMode = FilterMode::HP;
        registerBlockProcessor(&KorgonFilter::processBlockLP);
        break;
    }

    reset();
}

void KorgonFilter::setResonance(host_float reso)
{
    resonance = clamp(reso, 0.0, 100.0);
}

// Sets the filter drive
void KorgonFilter::setDrive(host_float value)
{
    drive = clamp(value, 0.0, 1.0) * 1.0 + 1.0;
}

void KorgonFilter::processBlockHP()
{
    host_float left, right;
    host_float cutoff;
    host_float wc, alpha;

    if (!std::isfinite(y1L))
        y1L = 0.0;
    if (!std::isfinite(y2L))
        y2L = 0.0;
    if (!std::isfinite(y1R))
        y1R = 0.0;
    if (!std::isfinite(y2R))
        y2R = 0.0;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        left = processBus.l[i];
        right = processBus.r[i];
        cutoff = modulationBus.m[i];

        if (cutoff > 15000.0)
        {
            processBus.l[i] = left;
            processBus.r[i] = right;
            continue;
        }

        wc = 2.0 * dsp_math::DSP_PI * cutoff;

        alpha = clamp(wc * T / (1.0 + wc * T), 0.0, 1.0);

        y1L += alpha * (left - y1L);
        y1R += alpha * (right - y1R);

        left = processBus.l[i] - y1L;
        right = processBus.r[i] - y1R;

        left = (left >= 0.0) ? dsp_math::fast_tanh(left) : 1.5 * dsp_math::fast_tanh(0.5 * left);
        right = (right >= 0.0) ? dsp_math::fast_tanh(right) : 1.5 * dsp_math::fast_tanh(0.5 * right);

        processBus.l[i] = left;
        processBus.r[i] = right;
    }
}

void KorgonFilter::processBlockLP()
{
    host_float left, right;
    host_float cutoff;
    host_float wc, alpha;
    host_float fbL, fbR;
    host_float xL, xR;
    host_float reso_scale;

    if (!std::isfinite(y1L))
        y1L = 0.0;
    if (!std::isfinite(y2L))
        y2L = 0.0;
    if (!std::isfinite(y1R))
        y1R = 0.0;
    if (!std::isfinite(y2R))
        y2R = 0.0;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        left = processBus.l[i];
        right = processBus.r[i];
        cutoff = modulationBus.m[i];

        if (cutoff > 15000.0)
        {
            processBus.l[i] = left;
            processBus.r[i] = right;
            continue;
        }

        reso_scale = (cutoff <= 2500.0) ? 1.0 : clamp(1.0 - (cutoff - 2500.0) / 7500.0, 0.0, 1.0);

        // Calculate coefficient based on cutoff
        wc = 2.0 * dsp_math::DSP_PI * cutoff;

        alpha = clamp(wc * T / (1.0 + wc * T), 0.0, 1.0); // Bilinear transform approximation

        // feedback calculation
        fbL = clamp(resonance * reso_scale * (y2L - left), -15.0, 15.0);
        fbR = clamp(resonance * reso_scale * (y2R - right), -15.0, 15.0);

        // First integrator (emulating Sallen-Key stage)
        xL = left - fbL;
        xR = right - fbR;

        y1L += alpha * (xL - y1L);
        y1R += alpha * (xR - y1R);

        // Second integrator
        y2L += alpha * (y1L - y2L);
        y2R += alpha * (y1R - y2R);

        // Apply asymmetric soft clip
        left = y2L * drive;
        right = y2R * drive;

        left = (left >= 0.0) ? dsp_math::fast_tanh(left) : 1.5 * dsp_math::fast_tanh(0.5 * left);
        right = (right >= 0.0) ? dsp_math::fast_tanh(right) : 1.5 * dsp_math::fast_tanh(0.5 * right);

        processBus.l[i] = left;
        processBus.r[i] = right;
    }
}

void KorgonFilter::processBlockLP(DSPObject *dsp)
{
    KorgonFilter *self = static_cast<KorgonFilter *>(dsp);

    self->processBlockLP();
}

void KorgonFilter::processBlockHP(DSPObject *dsp)
{
    KorgonFilter *self = static_cast<KorgonFilter *>(dsp);

    self->processBlockHP();
}

// Optional: reset internal state variables
void KorgonFilter::reset()
{
    y1L = 0.0;
    y2L = 0.0;
    y1R = 0.0;
    y2R = 0.0;
}