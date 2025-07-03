#include "ParamFader.h"
#include "DSP.h"

// Queue a parameter change
void ParamFader::change(ParamChange fn)
{
    changes.push(fn);
    applyParamChange = true;
}

void ParamFader::processChanges(DSPBuffer &left, DSPBuffer &right)
{
    if (applyParamChange)
    {
        fadeCounter++;

        if (fadeCounter <= fadeLength)
        {
            fadeValue = 1.0 - (dsp_float(fadeCounter) / fadeLength); // Fade out
        }
        else if (fadeCounter == fadeLength + 1)
        {
            // execute stored function
            while (!changes.empty())
            {
                changes.front()();
                changes.pop();
            }
        }
        else if (fadeCounter <= fadeLength * 2)
        {
            fadeValue = (dsp_float(fadeCounter - fadeLength) / fadeLength); // Fade in
        }
        else
        {
            applyParamChange = false;
            fadeValue = 1.0;
            fadeCounter = 0;
        }

        // Apply fade to entire output buffer
        for (size_t i = 0; i < DSP::blockSize; ++i)
        {
            left[i] *= fadeValue;
            right[i] *= fadeValue;
        }
    }
}

/*
if (applyOscillators)
{
    if (carrier != carrierTmp)
        carrier = carrierTmp;

    if (modulator != modulatorTmp)
        modulator = modulatorTmp;

    filter->reset();
}

if (applyDetune)
{
    carrier->setDetune(detune);
}
    */