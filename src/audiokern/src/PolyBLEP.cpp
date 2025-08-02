#include "PolyBLEP.h"

PolyBLEP::PolyBLEP()
{
    registerBlockProcessor(&PolyBLEP::processBlock);
}

PolyBLEP::~PolyBLEP()
{
}

void PolyBLEP::initializeProcessor()
{
    reset();    
    setFrequency(440.0f);
    reset();
}

void PolyBLEP::setFrequency(host_float freq)
{
    frequency = freq;
    phaseIncrement = frequency / DSP::sampleRate;
}

void PolyBLEP::reset()
{
    phase = 0.0f;
}

void PolyBLEP::processBlock()
{
    // Skip processing if frequency is effectively zero
    if (frequency <= 0.0f)
    {
        return; // Silent output - don't write anything to bus
    }
    
    for (size_t i = 0; i < DSP::blockSize; i++)
    {
        // Generate naive sawtooth wave (2 * phase - 1)
        host_float output = 2.0f * phase - 1.0f;
        
        // Apply PolyBLEP correction around discontinuities
        if (phase < phaseIncrement)
        {
            // Rising edge at phase wrap from 1.0 to 0.0
            host_float t = phase / phaseIncrement;
            output -= polyBlep(t);
        }
        else if (phase > 1.0f - phaseIncrement)
        {
            // Falling edge approaching phase wrap
            host_float t = (phase - 1.0f) / phaseIncrement;
            output -= polyBlep(t);
        }
        
        // Apply amplitude and write to output bus
        host_float amplitude = modulationBus.m[i];
        processBus.l[i] += output * amplitude;
        processBus.r[i] += output * amplitude;
        
        // Advance phase
        phase += phaseIncrement;
        
        // Wrap phase to [0, 1) range
        if (phase >= 1.0f)
        {
            phase -= 1.0f;
        }
    }
}

void PolyBLEP::processBlock(DSPObject *dsp)
{
    PolyBLEP *self = static_cast<PolyBLEP *>(dsp);
    self->processBlock();
}

inline host_float PolyBLEP::polyBlep(host_float t)
{
    if (t > 0.0f)
    {
        if (t <= 1.0f)
        {
            t = 2.0f * t - 1.0f;
            return -t * t * t / 3.0f + t * t - t / 3.0f - 1.0f / 24.0f;
        }
    }
    else
    {
        if (t >= -1.0f)
        {
            t = 2.0f * t + 1.0f;
            return t * t * t / 3.0f + t * t + t / 3.0f + 1.0f / 24.0f;
        }
    }
    
    return 0.0f;
}