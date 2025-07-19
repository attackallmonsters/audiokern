#pragma once

#include "DSPObject.h"
#include "CombDelay.h"
#include "clamp.h"
#include <vector>

// A stereo reverberation effect using multiple parallel comb filters.
// Parameters control density, delay times ("space"), damping and feedback amount.
class NebularReverb : public DSPObject
{
public:
    // Default constructor
    NebularReverb();

    // Called on each DSP activation to (re)initialize internal buffers and state
    void initialize() override;

    // Sets the number of parallel delay lines (2 to 8)
    void setDensity(int d);

    // Sets space amount 0 - 1 (controls the spread of the reverb)
    void setSpace(dsp_float size);

    // Sets the damping filter frequency 0 -20000 (typically a lowpass in the feedback path)
    void setDamping(dsp_float d);

    // Sets the room size (or tail length) 0 - 1 (ith a value of 1, the reverb will likely escalate)
    void setRoomSize(dsp_float size);

    // Feeds the reverb with samples
    //void setBuffers(DSPSampleBuffer &bufL, DSPSampleBuffer &bufR);

    // Processes the next block of samples
    void processBlock();

    // Output buffers
    DSPSampleBuffer outputBufferL;
    DSPSampleBuffer outputBufferR;

private:
    // Static DSP callback used for processing audio blocks
    static void processBlock(DSPObject *dsp);

    // Maximum number of comb delay lines
    static constexpr int maxDelays = 8;

    // Active number of delay lines (determined by density setting)
    int density = 4;

    // Base delay time in milliseconds (spread across delay lines)
    dsp_float delayTime = 40.0;

    // Damping filter cutoff frequency (Hz)
    dsp_float dampingFrequency = 4000.0;

    // Feedback gain factor (0.0 to <1.0)
    dsp_float feedback = 0.85;

    // Stereo comb delay lines
    std::vector<CombDelay> delays;

    // Updates all delay line parameters based on current settings
    void updateDelays();
};