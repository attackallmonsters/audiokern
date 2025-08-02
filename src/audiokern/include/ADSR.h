#pragma once

#include "clamp.h"
#include "Modulator.h"
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

class ADSR : public Modulator
{
public:
    ADSR();

    void setAttack(host_float ms);
    void setDecay(host_float ms);
    void setSustain(host_float level);
    void setRelease(host_float ms);
    void setAttackShape(host_float shape);
    void setReleaseShape(host_float shape);
    void setOneShot(bool b);
    void setStartAtCurrent(bool start);

    void triggerStart();
    void triggerStop();

protected:
    // Initializes the ADSR
    virtual void initializeModulator() override;

private:
    // Next sample block generation
    static void processBlock(DSPObject *dsp);

    // Phase function (startup, attack, decay, sustain, release)
    using PhaseFunc = void (ADSR::*)();
    PhaseFunc phaseFunc;

    // Envelope values
    host_float attackTime, decayTime, sustainLevel, releaseTime;
    host_float attackShape, releaseShape;
    host_float currentEnv, phaseStartEnv;
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

    static host_float powerLerp(host_float start, host_float end, host_float p, host_float shape);
    static host_float shapeToExponent(host_float f);

    host_float sampleRateMS;
    ADSRPhase phase;
    int currentSample;

    static constexpr host_float MAX_TIME = std::numeric_limits<host_float>::max();
};
