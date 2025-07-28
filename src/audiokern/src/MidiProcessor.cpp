#include "MidiProcessor.h"

void MidiProcessor::initialize()
{
    setLoudnessScale(0.7);
}

void MidiProcessor::setLoudnessScale(host_float scale)
{
    loudnessScale = clampmin(scale, 0.0001);
}

host_float MidiProcessor::normalizeVelocity(host_float velocity)
{
    return velocity * 0.00787402;
}

host_float MidiProcessor::normalizeVelocityRMS(host_float velocity)
{
    return pow(normalizeVelocity(velocity), loudnessScale);
}

host_float MidiProcessor::normalizePitchBend(host_float bend)
{
    return (bend - 8192.0) / 8192.0;
}

host_float MidiProcessor::normalizePitchBendRMS(host_float bend)
{
    return pow(normalizePitchBend(bend), loudnessScale);
}

host_float MidiProcessor::normalizeModulation(host_float mod)
{
    return mod / 127.0;
}

host_float MidiProcessor::normalizeModulationRMS(host_float mod)
{
    return pow(normalizeModulation(mod), loudnessScale);
}