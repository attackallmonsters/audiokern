#include "Limiter.h"

Limiter::Limiter()
{
    // to avoid vtable lookup in DSPObject
    registerBlockProcessor(&Limiter::processBlock);
}

// Initializes the instance
void Limiter::initializeProcessor()
{
    setThreshold(-3.0);
    setReleaseTime(20.0);
    setLookaheadTime(20.0);
    reset();
}

// Sets the amplitude threshold
void Limiter::setThreshold(dsp_float thres)
{
    threshold = std::pow(10.0, clamp(thres, -100.0, 0.0) / 20.0);
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
    lookaheadBuffer.assign(lookaheadSamples + 1, std::make_pair(0.0, 0.0));
    lookaheadSamples++;
}

// Resets the status
void Limiter::reset()
{
    std::fill(lookaheadBuffer.begin(), lookaheadBuffer.end(), std::make_pair(0.0f, 0.0f));

    bufferIndex = 0;
    gain = 1.0;
}

void Limiter::processBlock()
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Read current input sample (assuming input is written to outputBuffer)
        host_float inputL = processBus->l[i];
        host_float inputR = processBus->r[i];

        // Combine channels to get peak (for shared gain)
        host_float peak = std::max(std::fabs(inputL), std::fabs(inputR));
        host_float targetGain = (peak > threshold) ? threshold / peak : 1.0;

        // Fast attack / smooth release
        if (targetGain < gain)
            gain = targetGain;
        else
            gain += (1.0 - releaseCoeff) * (targetGain - gain);

        // Write to ring buffer and apply delayed gain
        lookaheadBuffer[bufferIndex] = {inputL, inputR};

        size_t readIndex = (bufferIndex + 1 + DSP::blockSize) % lookaheadSamples;
        bufferIndex = (bufferIndex + 1) % lookaheadSamples;

        auto &delayed = lookaheadBuffer[readIndex];

        processBus->l[i] = delayed.first * gain;
        processBus->r[i] = delayed.second * gain;
    }
}

// Processes the sample buffers
void Limiter::processBlock(DSPObject *dsp)
{
    // Cast to actual Limiter object
    Limiter *self = static_cast<Limiter *>(dsp);
    self->processBlock();
}
