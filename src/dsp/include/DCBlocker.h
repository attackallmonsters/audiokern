#pragma once

#include "dsp_types.h"

class DCBlocker
{
public:
    // Constructor with default R coefficient
    DCBlocker(dsp_float r = 0.995);

    // Resets the filter state
    void reset();

    // Sets the R coefficient (0.0 < r < 1.0)
    void setCoefficient(dsp_float r);

    // Processes a single sample
    dsp_float processSample(dsp_float input);

private:
    dsp_float x1; // Previous input sample x[n-1]
    dsp_float y1; // Previous output sample y[n-1]
    dsp_float R;  // Coefficient controlling low cut frequency
};
