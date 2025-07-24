#pragma once

#include "SoundProcessor.h"
#include "DSPObject.h"
#include "DSPSampleBuffer.h"
#include "SlewLimiter.h"
#include "dsp_math.h"
#include "clamp.h"
#include <cmath>

/**
 * @brief Enum to define the panning behavior for stereo signal processing
 */
enum class PanningMode
{
    Gain,       // Basic gain-based panning:
                // Each channel is scaled individually using sin/cos for equal-power,
                // but no signal is mixed between channels (no crossfeed).
                // Left stays left, right stays right — just louder or quieter.

    Blend,      // Stereo field crossfade panning /not phase save):
                // Both left and right input signals are mixed into both output channels,
                // using equal-power sin/cos gains.
                // Preserves stereo image while enabling smooth pan transitions with crossfeed.

    BlendMono   // Mono and phase save blend with equal-power panning:
                // Left and right inputs are summed to a mono signal,
                // then panned using sin/cos gains to both output channels.
                // Useful when the input should be centered and panned as a single source.
};

class Panner : public SoundProcessor
{
public:
    // Contructor
    Panner();

    // Sets panning amount: 0.0 = only left, 1.0 = only right
    void setPanning(double value);

    // Select simple gain mode or sample blending with equal power
    void setMode(PanningMode mode);

protected:
    // Initializes the insatnce
    void initializeProcessor() override;

private:
    // Block processing
    static void processBlockGain(DSPObject *dsp);
    static void processBlockBlend(DSPObject *dsp);
    static void processBlockBlendMono(DSPObject *dsp);

    void processBlockGain();
    void processBlockBlend();
    void processBlockBlendMono();

    // Audio artifact prevention
    SlewLimiter slew;

    double pan; // Raw mix value (0.0 to 1.0)

    DSPAudioBus *inputBus;
    DSPAudioBus *outputBus;
};
