#include "Limiter.h"

Limiter::Limiter()
{
    // to avoid vtable lookup in DSPObject
    registerBlockProcessor(&Limiter::processBlock);
}

// Initializes the instance
void Limiter::initialize()
{
    setThreshold(1.0);
    setReleaseTime(20.0);
    setLookaheadTime(20.0);
    reset();
}

// Sets the amplitude threshold
void Limiter::setThreshold(dsp_float thres)
{
    threshold = clampmin(thres, 0.0);
}

// Sets the release time in milliseconds
void Limiter::setReleaseTime(dsp_float releaseMs)
{
    // Calculate smoothing coefficient for exponential release
    releaseCoeff = std::exp(-1.0 / ((DSP::sampleRate / 1000.0) * releaseMs));
}

// Sets lookahead in milliseconds
void Limiter::setLookaheadTime(dsp_float ms)
{
    lookaheadSamples = static_cast<int>((DSP::sampleRate / 1000.0) * ms);
}

// Resets the status
void Limiter::reset()
{
    std::fill(lookaheadBuffer.begin(), lookaheadBuffer.end(), std::make_pair(0.0f, 0.0f));

    bufferIndex = 0;
    gain = 1.0;
}

// Processes the sample buffers
void Limiter::processBlock(DSPObject *dsp)
{
    // Cast to actual Limiter object
    Limiter *limiter = static_cast<Limiter *>(dsp);

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Read current input sample (assuming input is written to outputBuffer)
        host_float inputL = limiter->outputBufferL[i];
        host_float  inputR = limiter->outputBufferR[i];

        // Combine channels to get peak (for shared gain)
        host_float peak = std::max(std::fabs(inputL), std::fabs(inputR));
        host_float targetGain = (peak > limiter->threshold) ? limiter->threshold / peak : 1.0;

        // Fast attack / smooth release
        if (targetGain < limiter->gain)
            limiter->gain = targetGain;
        else
            limiter->gain += (1.0 - limiter->releaseCoeff) * (targetGain - limiter->gain);

        // Write to ring buffer and apply delayed gain
        limiter->lookaheadBuffer[limiter->bufferIndex] = {inputL, inputR};

        size_t readIndex = (limiter->bufferIndex + 1) % limiter->lookaheadBuffer.size();
        limiter->bufferIndex = (limiter->bufferIndex + 1) % limiter->lookaheadBuffer.size();

        auto& delayed = limiter->lookaheadBuffer[readIndex];

        limiter->outputBufferL[i] = delayed.first * limiter->gain;
        limiter->outputBufferR[i] = delayed.second * limiter->gain;
    }
}
