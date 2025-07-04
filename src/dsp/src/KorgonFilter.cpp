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

    setCutoffBuffer(cutoffInitBuffer);
    setResonanceBuffer(resoInitBuffer);
    setDrive(0.0);
    reset();

    T = 1.0 / DSP::sampleRate;
    drive = 1.0;
}

// Set the cutoff frequency and update coefficients
void KorgonFilter::setCutoffBuffer(DSPSampleBuffer &buffer)
{
    cutoffBuffer = buffer;
}

// Set the resonance amount
void KorgonFilter::setResonanceBuffer(DSPSampleBuffer &buffer)
{
    resoBuffer = buffer;
}

// Assigns the samples to process
void KorgonFilter::setSampleBuffers(DSPSampleBuffer &samplesL, DSPSampleBuffer &samplesR)
{
    bufferL = samplesL;
    bufferR = samplesR;
}

// Sets the filter drive
void KorgonFilter::setDrive(dsp_float value)
{
    drive = clamp(value, 0.0, 1.0) * 1.0 + 1.0;
}

// Process a single sample through the MS-20 style lowpass filter
void KorgonFilter::processBlock(DSPObject *dsp)
{
    KorgonFilter *flt = static_cast<KorgonFilter *>(dsp);

    size_t blocksize = DSP::blockSize;
    dsp_float left, right;
    dsp_float cutoff, reso;
    dsp_float wc, alpha;
    dsp_float feedback;
    dsp_float x1;
    dsp_float y1L = flt->y1L;
    dsp_float y2L = flt->y2L;
    dsp_float y1R = flt->y1R;
    dsp_float y2R = flt->y2R;
    dsp_float T = flt->T;
    dsp_float drive = flt->drive;
    dsp_float reso_scale;

    for (size_t i = 0; i < blocksize; ++i)
    {
        left = (flt->bufferL)[i];
        right = (flt->bufferR)[i];
        cutoff = clamp((flt->cutoffBuffer)[i], 0.0, 20000.0);
        reso = (flt->resoBuffer)[i];

        if (cutoff > 15000.0)
        {
            (flt->bufferL)[i] = left;
            (flt->bufferR)[i] = right;
            continue;
        }

        reso_scale = (cutoff <= 2500.0) ? 1.0 : clamp(1.0 - (cutoff - 2500.0) / 7500.0, 0.0, 1.0);

        // Calculate coefficient based on cutoff
        wc = 2.0 * M_PI * cutoff;

        alpha = clamp(wc * T / (1.0 + wc * T), 0.0, 1.0); // Bilinear transform approximation

        // === left ===
        feedback = clamp(reso * reso_scale * (y2L - left), -15.0, 15.0);

        // First integrator (emulating Sallen-Key stage)
        x1 = left - feedback;

        y1L += alpha * (x1 - y1L);

        // Second integrator
        y2L += alpha * (y1L - y2L);

        // Apply asymmetric soft clip
        left = y2L * drive;
        left = (left >= 0.0) ? fast_tanh(left) : 1.5 * fast_tanh(0.5 * left);
        (flt->bufferL)[i] = left;

        // === right ===
        feedback = clamp(reso * reso_scale * (y2R - right), -15.0, 15.0);

        // First integrator (emulating Sallen-Key stage)
        x1 = right - feedback;
        y1R += alpha * (x1 - y1R);

        // Second integrator
        y2R += alpha * (y1R - y2R);

        // Apply asymmetric soft clip
        right = y2R * drive;
        right = (right >= 0.0) ? fast_tanh(right) : 1.5 * fast_tanh(0.5 * right);
        (flt->bufferR)[i] = right;
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