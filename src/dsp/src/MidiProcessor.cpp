#include "MidiProcessor.h"

void MidiProcessor::initialize()
{
    
}

dsp_float MidiProcessor::noteToFrequency(int note)
{
    return tuningSystem.calculateIntervallFrequency(note);
}