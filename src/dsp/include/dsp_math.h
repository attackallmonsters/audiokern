#pragma once

// The PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline dsp_float fast_tanh(dsp_float val)
{
    if (val < -3.0)
        return -1.0;
    if (val > 3.0)
        return 1.0;

    const dsp_float val2 = val * val;
    return val * (27.0 + val2) / (27.0 + 9.0 * val2);
}

inline dsp_float soft_clip(dsp_float x)
{
    const dsp_float threshold = 2.5;
    if (x < -threshold)
        return -1.0;
    else if (x > threshold)
        return 1.0;
    else
        return x * (1.0 - (x * x) / (3.0 * threshold * threshold));
}