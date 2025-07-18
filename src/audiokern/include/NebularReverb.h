#pragma once

#include "DSPObject.h"
#include "CombDelay.h"
#include "clamp.h"
#include <vector>

class NebularReverb : public DSPObject
{
public:
    NebularReverb();

    void initialize() override;
    static void processBlock(DSPObject* dsp);

    // Parameter setter
    void setDensity(int d);
    void setSpace(dsp_float size);
    void setDamping(dsp_float d);
    void setSpaceSize(dsp_float size);

private:
    static constexpr int maxDelays = 8;
    int density;
    dsp_float delayTime;
    dsp_float dampingFrequency;
    dsp_float feedback;

    std::vector<CombDelay> delaysL;
    std::vector<CombDelay> delaysR;

    void updateDelays();
};
