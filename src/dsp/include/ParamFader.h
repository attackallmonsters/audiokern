#pragma once

#include <functional>
#include <queue>
#include "DSPBuffer.h"
#include "dsp_types.h"

class ParamFader
{
public:
    using ParamChange = std::function<void()>;

    // Queue a parameter change: paramFader.change([=]() { carrier->setDetune(detune); });
    void change(ParamChange fn);

    // Apply all queued changes
    void processChanges(DSPBuffer &left, DSPBuffer &right);

private:
    std::queue<ParamChange> changes;
    int fadeCounter = 0;           // Amplitude for param change
    const int fadeLength = 16;     // Fade in/out samples for param change
    dsp_float fadeValue = 1.0;     // Current amplitude for param change
    bool applyParamChange = false; // Indicates a param to change
};