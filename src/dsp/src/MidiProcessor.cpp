#include "MidiProcessor.h"

void MidiProcessor::initialize()
{
    setLoudnessScale(0.7);
}

void MidiProcessor::setLoudnessScale(host_float s)
{
    loudnessScale = clampmin(s, 0.0001);
}

host_float MidiProcessor::rmsVelocity(int velocity)
{
    return pow(static_cast<host_float>(velocity) * 0.00787402, loudnessScale);
}