// === KorgonFilter.cpp ===
#include "KorgonFilter.h"

// Constructor with sample rate
KorgonFilter::KorgonFilter()
{
    registerBlockProcessor(&KorgonFilter::processBlock);
}

// Initializes the filter
void KorgonFilter::initialize()
{
    DSPObject::initialize();

    cutoffInitBuffer.create(DSP::blockSize);
    resoInitBuffer.create(DSP::blockSize);

    cutoffInitBuffer.fill(20000.0);
    resoInitBuffer.fill(0.0);

    cutoffBuffer = cutoffInitBuffer;
    resoBuffer = resoInitBuffer;

    setDrive(0.0);
    reset();

    T = 1.0 / DSP::sampleRate;
    drive = 1.0;
}

void KorgonFilter::setFilterMode(FilterMode mode)
{
    switch (mode)
    {
    case FilterMode::LP:
        filterMode = FilterMode::LP;
        break;
    case FilterMode::HP:
        filterMode = FilterMode::HP;
        break;
    default:
        filterMode = FilterMode::LP;
        break;
    }

    reset();
}

// Sets the filter drive
void KorgonFilter::setDrive(host_float value)
{
    drive = clamp(value, 0.0, 1.0) * 1.0 + 1.0;
}

// Process a single sample through the MS-20 style lowpass filter
void KorgonFilter::processBlock(DSPObject *dsp)
{
    KorgonFilter *flt = static_cast<KorgonFilter *>(dsp);

    size_t blocksize = DSP::blockSize;
    host_float left, right;
    host_float cutoff, reso;
    host_float wc, alpha;
    host_float fbL, fbR;
    host_float xL, xR;
    host_float y1L = flt->y1L;
    host_float y2L = flt->y2L;
    host_float y1R = flt->y1R;
    host_float y2R = flt->y2R;
    host_float T = flt->T;
    host_float drive = flt->drive;
    host_float reso_scale;

    if (!std::isfinite(y1L))
        y1L = 0.0;
    if (!std::isfinite(y2L))
        y2L = 0.0;
    if (!std::isfinite(y1R))
        y1R = 0.0;
    if (!std::isfinite(y2R))
        y2R = 0.0;

    if (flt->filterMode == FilterMode::LP)
    {
        for (size_t i = 0; i < blocksize; ++i)
        {
            left = (flt->outputBufferL)[i];
            right = (flt->outputBufferR)[i];
            cutoff = (flt->cutoffBuffer)[i];
            reso = (flt->resoBuffer)[i];

            if (cutoff > 15000.0)
            {
                (flt->outputBufferL)[i] = left;
                (flt->outputBufferR)[i] = right;
                continue;
            }

            reso_scale = (cutoff <= 2500.0) ? 1.0 : clamp(1.0 - (cutoff - 2500.0) / 7500.0, 0.0, 1.0);

            // Calculate coefficient based on cutoff
            wc = 2.0 * M_PI * cutoff;

            alpha = clamp(wc * T / (1.0 + wc * T), 0.0, 1.0); // Bilinear transform approximation

            // feedback calculation
            fbL = clamp(reso * reso_scale * (y2L - left), -15.0, 15.0);
            fbR = clamp(reso * reso_scale * (y2R - right), -15.0, 15.0);

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

            //left = (left >= 0.0) ? fast_tanh(left) : 1.5 * fast_tanh(0.5 * left);
            //right = (right >= 0.0) ? fast_tanh(right) : 1.5 * fast_tanh(0.5 * right);

            (flt->outputBufferL)[i] = left;
            (flt->outputBufferR)[i] = right;
        }
    }
    else
    {
        for (size_t i = 0; i < blocksize; ++i)
        {
            left = (flt->outputBufferL)[i];
            right = (flt->outputBufferR)[i];
            cutoff = (flt->cutoffBuffer)[i];

            if (cutoff > 15000.0)
            {
                (flt->outputBufferL)[i] = left;
                (flt->outputBufferR)[i] = right;
                continue;
            }

            // Calculate coefficient based on cutoff
            wc = 2.0 * M_PI * cutoff;

            alpha = clamp(wc * T / (1.0 + wc * T), 0.0, 1.0); // Bilinear transform approximation

            y1L += alpha * (left - y1L);
            y1R += alpha * (right - y1R);

            left = (flt->outputBufferL)[i] - y1L;
            right = (flt->outputBufferR)[i] - y1R;

            left = (left >= 0.0) ? fast_tanh(left) : 1.5 * fast_tanh(0.5 * left);
            right = (right >= 0.0) ? fast_tanh(right) : 1.5 * fast_tanh(0.5 * right);

            (flt->outputBufferL)[i] = left;
            (flt->outputBufferR)[i] = right;
        }
    }

    flt->y1L = y1L;
    flt->y2L = y2L;
    flt->y1R = y1R;
    flt->y2R = y2R;
}

// Optional: reset internal state variables
void KorgonFilter::reset()
{
    y1L = 0.0;
    y2L = 0.0;
    y1R = 0.0;
    y2R = 0.0;
}