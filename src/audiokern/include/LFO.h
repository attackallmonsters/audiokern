#pragma once

#include "DSPObject.h"
#include "DSPSampleBuffer.h"
#include "dsp_types.h"
#include "dsp_math.h"
#include "clamp.h"
#include <cmath>
#include <cstdlib>
#include <functional>

enum class LFOType {
    Sine,
    RampUp,
    RampDown,
    Triangle,
    Square,
    Random
};

enum class LFOMode
{
    Buffered,
    Value
};

class LFO : public DSPObject
{
public:
    LFO();

    virtual void initialize() override;

    void setFreq(host_float f);
    void setType(LFOType type);
    void setOffset(host_float f);
    void setDepth(host_float f);
    void setShape(host_float f);
    void setPulseWidth(host_float f);
    void setSmooth(host_float f);
    void setIdleSignal(host_float f);
    void isUnipolar(bool enabled);
    void setMode(LFOMode mode);
    void reset();

    // Called on phase wrap
    std::function<void()> onPhaseWrap = nullptr;

    // Called for first buffer element each block 
    std::function<void(host_float)> processLFOValue = nullptr;

    // Output biffer
    DSPSampleBuffer outputBuffer;

    // FM modulation buffer
    DSPSampleBuffer modulationBuffer;

private:
    static void processBlockBuffer(DSPObject* dsp);
    static void processBlockValue(DSPObject* dsp);

    // Current sample rate derived values
    host_float phase;
    host_float freq;
    host_float phaseInc;
    host_float samplerate;
    host_float offset;
    host_float depth;
    host_float shape;
    host_float pw;
    host_float smoothVal;
    host_float smoothCoeff;
    host_float idleSignal;
    bool unipolar;
    LFOMode lfoMode;

    DSPSampleBuffer modBufferDefault;

    LFOType lfoType;

    // Waveshaper
    host_float shapedRamp(host_float x);

    // Internal waveform calculation functions
    host_float lfoSine();
    host_float lfoRampUp();
    host_float lfoRampDown();
    host_float lfoTriangle();
    host_float lfoSquare();
    host_float lfoRandom();

    bool fmEnabled;

    host_float (LFO::*lfoFunc)() = nullptr;
};