#pragma once

#include "clamp.h"
#include "DSPObject.h"
#include "DSPBuffer.h"
#include <cmath>

// Enumeration for the envelope phase
enum class ADSRPhase
{
    Idle,
    Startup,
    Attack,
    Decay,
    Sustain,
    Release
};

class ADSR : public DSPObject
{
public:
    ADSR();
    
    // Initializes the ADSR
    virtual void initialize() override;

    void setAttack(dsp_float ms);
    void setDecay(dsp_float ms);
    void setSustain(dsp_float level);
    void setRelease(dsp_float ms);
    void setAttackShape(dsp_float shape);
    void setReleaseShape(dsp_float shape);
    void setGain(dsp_float g);
    void setOneShot(bool b);
    void setStartAtCurrent(bool start);

    dsp_float* getBuffer();

    void triggerStart();
    void triggerStop();

private:
    // Next sample block generation
    static void processBlock(DSPObject *dsp);

    // Phase function (startup, attack, decay, sustain, release)
    using PhaseFunc = void (ADSR::*)();
    PhaseFunc phaseFunc;

    // Envelope values
    dsp_float attackTime, decayTime, sustainLevel, releaseTime;
    dsp_float attackShape, releaseShape;
    dsp_float currentEnv, phaseStartEnv;
    dsp_float gain;
    bool oneShot;
    bool startAtCurrentEnv;

    int attackSamples, decaySamples, releaseSamples, startupSamples;

    static constexpr int startupTimeMS = 3;

    void enterPhase(ADSRPhase newPhase);

    void phaseIdle();
    void phaseStartup();
    void phaseAttack();
    void phaseDecay();
    void phaseSustain();
    void phaseRelease();

    static dsp_float powerLerp(dsp_float start, dsp_float end, dsp_float p, dsp_float shape);
    static dsp_float shapeToExponent(dsp_float f);

    dsp_float sampleRateMS;
    ADSRPhase phase;
    int currentSample;
    DSPBuffer curveBuffer;

    static constexpr dsp_float MAX_TIME = std::numeric_limits<dsp_float>::max();
};
