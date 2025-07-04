#pragma once

#include "VoiceOptions.h"
#include "DSP.h"
#include "DSPObject.h"
#include "dsp_types.h"
#include "dsp_math.h"
#include "clamp.h"
#include <cmath>
#include <algorithm>

enum class FilterStage
{
    TwoPole,
    FourPole
};

// A zero-delay feedback (ZDF) multimode filter using the TPT (Topology-Preserving Transform) structure.
// This filter provides 12 dB/oct lowpass, highpass, and bandpass outputs, switchable via setMode().
class LadderFilter : public DSPObject
{
public:
    // Constructor: sets sample rate and initializes internal state and coefficients
    LadderFilter();

    // Set the cutoff frequency in Hz (limited to Nyquist range)
    void setCutoff(dsp_float freq);

    // Set the resonance amount (typically 0.0 to ~4.0 for self-oscillation)
    void setResonance(dsp_float res);

    // Set the drive amount
    void setDrive(dsp_float drv);

    // Choose filter mode: LPF12, BPF12, or HPF12
    void setMode(FilterMode mode);

    // Sets the filter stage
    void setFilterStage(FilterStage stage);

    // Reset the internal state variables
    void reset();

    // The samples to be filtered
    XDSPBuffer *bufferL;
    XDSPBuffer *bufferR;
private:
    // Next sample block generation
    static void processBlock(DSPObject *dsp);

    FilterStage filterStage; // filter calculation stages

    // --- Parameters ---
    dsp_float cutoff;         // Cutoff frequency in Hz
    dsp_float resonance;      // Resonance amount (Q control)
    dsp_float drive;          // filter drive
    FilterMode filterMode; // Selected filter mode

    // --- Internal state ---
    dsp_float s1L, s2L, s3L, s4L;
    dsp_float s1R, s2R, s3R, s4R;
};
