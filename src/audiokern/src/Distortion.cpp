#include "Distortion.h"
#include "clamp.h"
#include <cmath>

Distortion::Distortion()
{
    // Initial registration - will be updated when distortion type changes
    registerEffectBlockProcessor(&Distortion::processSoftClip);
}

void Distortion::initializeEffect()
{
    filter.initialize("toneFilter" + getName());

    setDrive(0.0f);
    setOutputGain(1.0f);
    setDistortionType(DistortionType::SoftClip);
    setTone(0.0f);
}

void Distortion::initializeEffect(size_t /*count*/)
{
    initializeEffect();
}

void Distortion::onInputBusConnected(DSPAudioBus &bus)
{
    filter.connectProcessToBus(bus);
}

void Distortion::setDrive(host_float d)
{
    drive = clamp(d, 0.0f, 1.0f) * 20.0f;
}

void Distortion::setOutputGain(host_float gain)
{
    outputGain = clamp(gain, 0.0f, 1.0f);
}

void Distortion::setDistortionType(DistortionType type)
{
    distortionType = type;

    switch (distortionType)
    {
    case DistortionType::SoftClip:
        registerEffectBlockProcessor(&Distortion::processSoftClip);
        break;
    case DistortionType::HardClip:
        registerEffectBlockProcessor(&Distortion::processHardClip);
        break;
    case DistortionType::Tube:
        registerEffectBlockProcessor(&Distortion::processTube);
        break;
    case DistortionType::Foldback:
        registerEffectBlockProcessor(&Distortion::processFoldback);
        break;
    }
}

void Distortion::setTone(host_float t)
{
    tone = clamp(t, -1.0f, 1.0f);

    if (t >= 0.05)
    {
        filter.setFilterMode(FilterMode::LP);
        filter.setCutoffFrequency(5000.0f - t * 4900.0f);
    }
    else if (t < 0.05f)
    {
        filter.setFilterMode(FilterMode::HP);
        filter.setCutoffFrequency(std::abs(t) * 5000.0f);
    }
    else
    {
        filter.setFilterMode(FilterMode::LP);
        filter.setCutoffFrequency(20000.0f);
        return;
    }
}

void Distortion::processSoftClip()
{
    // Filter input signal
    filter.process();

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Calculate effective drive amount with modulation
        host_float driveMultiplier = 1.0f + drive * (1.0f + modulationBusA.m[i]);

        // Apply drive and soft clipping (tanh)
        host_float drivenL = inputBus.l[i] * driveMultiplier;
        host_float distortedL = std::tanh(drivenL);

        // Apply drive and soft clipping (tanh)
        host_float drivenR = inputBus.r[i] * driveMultiplier;
        host_float distortedR = std::tanh(drivenR);

        // Apply output gain and write to output bus
        wetBus.l[i] = distortedL * outputGain;
        wetBus.r[i] = distortedR * outputGain;
    }
}

void Distortion::processHardClip()
{
    // Filter input signal
    filter.process();

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Calculate effective drive amount with modulation - much more aggressive for hard clipping
        host_float driveMultiplier = 1.0f + drive * (1.0f + modulationBusA.m[i]) * 2.0f; // 2x more aggressive

        // Apply drive with extra boost for harder character
        host_float drivenL = inputBus.l[i] * driveMultiplier;

        // Hard clipping with lower threshold for more aggressive clipping
        host_float threshold = 0.7f; // Lower threshold = more clipping
        host_float distortedL;
        if (drivenL > threshold)
            distortedL = threshold;
        else if (drivenL < -threshold)
            distortedL = -threshold;
        else
            distortedL = drivenL;

        // Apply drive with extra boost for harder character
        host_float drivenR = inputBus.r[i] * driveMultiplier;

        // Hard clipping with lower threshold
        host_float distortedR;
        if (drivenR > threshold)
            distortedR = threshold;
        else if (drivenR < -threshold)
            distortedR = -threshold;
        else
            distortedR = drivenR;

        // Apply output gain and write to output bus
        wetBus.l[i] = distortedL * outputGain;
        wetBus.r[i] = distortedR * outputGain;
    }
}

void Distortion::processTube()
{
    // Filter input signal
    filter.process();

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Calculate effective drive amount with modulation - moderate for tube warmth
        host_float driveMultiplier = 1.0f + drive * (1.0f + modulationBusA.m[i]) * 0.5f; // Gentler drive

        // Apply gentle drive for tube character
        host_float drivenL = inputBus.l[i] * driveMultiplier;

        // Tube saturation with much more pronounced asymmetry
        host_float distortedL;
        if (drivenL >= 0.0f)
        {
            // Positive half: very soft compression (tube "sag")
            distortedL = drivenL / (1.0f + drivenL * drivenL * 0.3f); // Square term for softer curve
        }
        else
        {
            // Negative half: much harder compression for strong asymmetry
            host_float absDriven = -drivenL;
            distortedL = -(absDriven / (1.0f + absDriven * 2.5f)); // Much stronger compression
        }

        // Apply gentle drive for tube character
        host_float drivenR = inputBus.r[i] * driveMultiplier;

        // Tube saturation with asymmetry
        host_float distortedR;
        if (drivenR >= 0.0f)
        {
            // Positive half: very soft compression
            distortedR = drivenR / (1.0f + drivenR * drivenR * 0.3f);
        }
        else
        {
            // Negative half: much harder compression
            host_float absDriven = -drivenR;
            distortedR = -(absDriven / (1.0f + absDriven * 2.5f));
        }

        // Apply output gain and write to output bus
        wetBus.l[i] = distortedL * outputGain;
        wetBus.r[i] = distortedR * outputGain;
    }
}

void Distortion::processFoldback()
{
    const host_float threshold = 1.0f;

    // Filter input signal
    filter.process();

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Calculate effective drive amount with modulation
        host_float driveMultiplier = 1.0f + drive * (1.0f + modulationBusA.m[i]);

        // Apply drive and wave folding
        host_float drivenL = inputBus.l[i] * driveMultiplier;
        host_float distortedL = drivenL;

        // Inline wave folding
        if (std::abs(distortedL) > threshold)
        {
            while (std::abs(distortedL) > threshold)
            {
                if (distortedL > threshold)
                {
                    distortedL = 2.0f * threshold - distortedL;
                }
                else if (distortedL < -threshold)
                {
                    distortedL = -2.0f * threshold - distortedL;
                }
            }
        }

        // Apply drive and wave folding
        host_float drivenR = inputBus.r[i] * driveMultiplier;
        host_float distortedR = drivenR;

        // Inline wave folding
        if (std::abs(distortedR) > threshold)
        {
            while (std::abs(distortedR) > threshold)
            {
                if (distortedR > threshold)
                {
                    distortedR = 2.0f * threshold - distortedR;
                }
                else if (distortedR < -threshold)
                {
                    distortedR = -2.0f * threshold - distortedR;
                }
            }
        }

        // Apply output gain and write to output bus
        wetBus.l[i] = distortedL * outputGain;
        wetBus.r[i] = distortedR * outputGain;
    }
}

// Static dispatchers
void Distortion::processSoftClip(DSPObject *dsp)
{
    Distortion *self = static_cast<Distortion *>(dsp);
    self->processSoftClip();
}

void Distortion::processHardClip(DSPObject *dsp)
{
    Distortion *self = static_cast<Distortion *>(dsp);
    self->processHardClip();
}

void Distortion::processTube(DSPObject *dsp)
{
    Distortion *self = static_cast<Distortion *>(dsp);
    self->processTube();
}

void Distortion::processFoldback(DSPObject *dsp)
{
    Distortion *self = static_cast<Distortion *>(dsp);
    self->processFoldback();
}