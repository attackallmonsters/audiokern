#include "LadderFilter.h"

// Konstruktor mit Defaultwerten
LadderFilter::LadderFilter()
{
    s1L = s2L = s3L = s4L = 0.0;
    s1R = s2R = s3R = s4R = 0.0;
    cutoff = 15000.0;
    resonance = 0.0;
    drive = 1.0;
    filterStage = FilterStage::TwoPole;

    // to avoid vtable lookup
    registerBlockProcessor(&LadderFilter::processBlock);
}

// Set cutoff frequency in Hz
void LadderFilter::setCutoff(dsp_float freq)
{
    cutoff = clamp(freq, 1.0, sampleRate * 0.45);
}

// Set resonance
void LadderFilter::setResonance(dsp_float res)
{
    resonance = clamp(res, 0.0, 6.0);
}

// Set resonance
void LadderFilter::setDrive(dsp_float drv)
{
    drive = clamp(drv, 1.0, 20.0);
}

// Reset filter status
void LadderFilter::reset()
{
    s1L = s2L = s3L = s4L = 0.0;
    s1R = s2R = s3R = s4R = 0.0;
}

// Sets the filter stage
void LadderFilter::setFilterStage(FilterStage stage)
{
    filterStage = stage;
}

// Next sample block generation
void LadderFilter::processBlock(DSPObject *dsp)
{
    LadderFilter *flt = static_cast<LadderFilter *>(dsp);

    // Load previous filter states
    dsp_float s1L = flt->s1L, s2L = flt->s2L, s3L = flt->s3L, s4L = flt->s4L;
    dsp_float s1R = flt->s1R, s2R = flt->s2R, s3R = flt->s3R, s4R = flt->s4R;

    // Compute g from the cutoff frequency (normalized angular frequency)
    dsp_float g = tan(M_PI * flt->cutoff / DSP::sampleRate);

    // Compute the smoothing coefficient (1-pole lowpass response)
    dsp_float alpha = g / (1.0 + g);

    // Gain copmpensation
    dsp_float compensation;
    if (flt->filterStage == FilterStage::TwoPole)
        compensation = 1.0 / std::pow(1.0 + g, 0.5);
    else
        compensation = 1.0 / std::pow(1.0 + g, 1.0);

    size_t blocksize = DSP::blockSize;
    dsp_float left;
    dsp_float right;

    for (size_t i = 0; i < blocksize; ++i)
    {
        left = (*flt->bufferL)[i];
        right = (*flt->bufferR)[i];

        // Feedback calculation
        dsp_float inputL, inputR;
        if (flt->filterStage == FilterStage::TwoPole)
        {
            inputL = fast_tanh(flt->drive * 0.5 * left) - flt->resonance * fast_tanh(flt->drive * s2L);
            inputR = fast_tanh(flt->drive * 0.5 * right) - flt->resonance * fast_tanh(flt->drive * s2R);
        }
        else
        {
            inputL = fast_tanh(flt->drive * 0.5 * left) - flt->resonance * fast_tanh(flt->drive * s4L);
            inputR = fast_tanh(flt->drive * 0.5 * right) - flt->resonance * fast_tanh(flt->drive * s4R);
        }

        s1L += alpha * (inputL - s1L);
        s2L += alpha * (s1L - s2L);
        s1R += alpha * (inputR - s1R);
        s2R += alpha * (s1R - s2R);

        if (flt->filterStage == FilterStage::FourPole)
        {
            s3L += alpha * (s2L - s3L);
            s4L += alpha * (s3L - s4L);
            s3R += alpha * (s2R - s3R);
            s4R += alpha * (s3R - s4R);
        }

        if (flt->filterStage == FilterStage::TwoPole)
        {
            (*flt->bufferL)[i] = fast_tanh(s2L * compensation);
            (*flt->bufferR)[i] = fast_tanh(s2R * compensation);
        }
        else
        {
            (*flt->bufferL)[i] = fast_tanh(s4L * compensation);
            (*flt->bufferR)[i] = fast_tanh(s4R * compensation);
        }
    }

    flt->s2L = s2L;
    flt->s3L = s3L;
    flt->s4L = s4L;
    flt->s1R = s1R;
    flt->s2R = s2R;
    flt->s3R = s3R;
    flt->s4R = s4R;
}
