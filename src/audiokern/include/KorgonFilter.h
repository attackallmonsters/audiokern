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

    // Initializes the filter
    void initialize() override;

    // Sets the filter drive
    void setDrive(host_float value);

    // Sets the filter mode (LP, HP)
    void setFilterMode(FilterMode mode);

    // The samples to be filtered
    DSPSampleBuffer processBufferL;
    DSPSampleBuffer processBufferR;

    // Buffer for cutoff calculation
    DSPSampleBuffer cutoffBuffer;

    // Buffer for resonance
    DSPSampleBuffer resoBuffer;

    // Reset internal filter state
    void reset();

private:
    // Filter state variables:
    host_float y1L;   // Output of first integrator left
    host_float y2L;   // Output of second integrator (filter output) left
    host_float y1R;   // Output of first integrator right
    host_float y2R;   // Output of second integrator (filter output) right
    host_float T;     // Simplified impulse invariant/bilinear transformation
    host_float drive; // The filter drive

    static host_float nonlinearFeedback(host_float s); // Nonlinear feedback (simulates diode behavior)

    // Processes data in bufferL, buffer R
    static void processBlock(DSPObject *dsp);

    // Control buffer for cutoff
    DSPSampleBuffer cutoffInitBuffer;

    // Control buffer for resonance    
    DSPSampleBuffer resoInitBuffer;

    // Filtermode, Korgon supports LP and HP
    FilterMode filterMode;
};
