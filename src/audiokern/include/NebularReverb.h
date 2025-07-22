#pragma once

#include "DSPObject.h"
#include "CombDelay.h"
#include "clamp.h"
#include <array>
#include <vector>

// A stereo reverberation effect using multiple parallel comb filters.
// Parameters control density, delay times ("space"), damping and feedback amount.
// This reverb has a more metallic sound, not unlike a Karplus algorithm or plate.
// Because it works with direct and relatively short feedback times.
// However, you can vary the feedback times so that the metallic sound is better
// dampened by the reverb cloud.
// For the best sound configuration, the feedback (room size) should be very finely
// configurable in the upper range 90 - 100%. The reflections (space) should be finely
// configurable in the lower range 0 - 10 %.
class NebularReverb : public DSPObject
{
public:
    // Default constructor
    NebularReverb();

    // Sets the number of parallel delay lines (2 to 8)
    void setDensity(host_float dense);

    // Sets space amount 0 - 1 (controls the spread of the reverb)
    void setSpace(host_float size);

    // Sets the damping filter frequency 0 -20000 (typically a lowpass in the feedback path)
    void setDamping(host_float d);

    // Sets the room size (or tail length) 0 - 1 (ith a value of 1, the reverb will likely escalate)
    void setRoomSize(host_float size);

    // Sets the output volume
    void setVolume(host_float vol);

    // Processes the next block of samples
    void processBlock();

protected:
    // Component initialization on DSP activation
    DSPObjectUsage initializeComponent() override;

private:
    // Static DSP callback used for processing audio blocks
    static void processBlock(DSPObject *dsp);

    // Pushes samples inputBufferL/R to the ring buffers
    void push();

    // Maximum number of comb delay lines
    static constexpr int maxDelays = 8;

    // Active number of delay lines (determined by density setting)
    int density, prevDensity;

    // Base delay time in milliseconds (spread across delay lines)
    host_float delayTime, prevDelayTime;

    // Controls the output volume of the reverb
    host_float volume;

    // Stereo comb delay lines
    std::array<CombDelay, maxDelays> delays;

    // Delays output buffers
    std::vector<DSPBus> delayBusses;

    // Updates all delay line parameters based on current settings
    void updateDelays();
};