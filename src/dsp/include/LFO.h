#pragma once

#include "DSPObject.h"
#include "XDSPBuffer.h"
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

class LFO : public DSPObject
{
public:
    LFO();

    virtual void initialize() override;

    void setFreq(dsp_float f);
    void setType(LFOType type);
    void setOffset(dsp_float f);
    void setDepth(dsp_float f);
    void setShape(dsp_float f);
    void setPulseWidth(dsp_float f);
    void setSmooth(dsp_float f);
    void setIdleSignal(dsp_float f);
    void reset();
    dsp_float* getBuffer();

    std::function<void()> onPhaseWrap;

private:
    static void processBlock(DSPObject* dsp);

    // Current sample rate derived values
    dsp_float phase;
    dsp_float freq;
    dsp_float phaseInc;
    dsp_float samplerate;
    dsp_float offset;
    dsp_float depth;
    dsp_float shape;
    dsp_float pw;
    dsp_float smoothVal;
    dsp_float smoothCoeff;
    dsp_float idleSignal;

    XDSPBuffer lfoBuffer;
    XDSPBuffer modBuffer;
    XDSPBuffer modBufferDefault;

    LFOType lfoType;

    // Internal waveform calculation functions
    dsp_float lfoSine();
    dsp_float lfoRampUp();
    dsp_float lfoRampDown();
    dsp_float lfoTriangle();
    dsp_float lfoSquare();
    dsp_float lfoRandom();
    dsp_float shapedRamp(dsp_float x);
    bool fmEnabled;

    dsp_float (LFO::*lfoFunc)() = nullptr;
};