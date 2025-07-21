#include "SineWavetable.h"

void SineWavetable::createWavetable(DSPBuffer &buffer, dsp_float /*frequency*/)
{
    size_t size = buffer.size();

    // Fill one full waveform cycle (0 to 2π) across the buffer
    for (size_t i = 0; i < size; ++i)
    {
        double phase = static_cast<double>(i) / static_cast<double>(size);
        buffer[i] = std::sin(2.0 * dsp_math::DSP_PI * phase);
    }
}
