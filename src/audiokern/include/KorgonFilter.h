#pragma once

#include "DSPObject.h"
#include "DSPSampleBuffer.h"
#include "VoiceOptions.h"
#include "dsp_types.h"
#include "clamp.h"
#include "dsp_math.h"
#include <cmath>

class KorgonFilter : public DSPObject
{
public:
    // Constructor with sample rate
    explicit KorgonFilter();

    // Sets the filter drive
    void setDrive(host_float value);

    // Sets the filter mode (LP, HP)
    void setFilterMode(FilterMode mode);

    // Sets the filters resonance
    void setResonance(host_float reso);

    // Processes the next block of samples
    void processBlock();

    // Reset internal filter state
    void reset();

protected:
    // Initializes the filter
    DSPObjectUsage initializeComponent() override;

private:
    // Static DSP callback used for processing audio blocks
    static void processBlock(DSPObject *dsp);

    // Filter state variables:
    host_float y1L;   // Output of first integrator left
    host_float y2L;   // Output of second integrator (filter output) left
    host_float y1R;   // Output of first integrator right
    host_float y2R;   // Output of second integrator (filter output) right
    host_float T;     // Simplified impulse invariant/bilinear transformation
    host_float drive; // The filter drive

    static host_float nonlinearFeedback(host_float s); // Nonlinear feedback (simulates diode behavior)

    // Filtermode, Korgon supports LP and HP
    FilterMode filterMode;

    // Filter resonance
    host_float resonance;
};
