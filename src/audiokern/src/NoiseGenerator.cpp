#include "NoiseGenerator.h"
#include "dsp_types.h"
#include "dsp_math.h"

// Constructor: seeds RNG and sets noise type
NoiseGenerator::NoiseGenerator()
{
    registerBlockProcessor(&NoiseGenerator::processBlockWhite);
}

void NoiseGenerator::initializeGenerator()
{
    setType(NoiseType::White);
}

// Allows dynamic switching between white and pink noise
void NoiseGenerator::setType(NoiseType type)
{
    switch (type)
    {
    case NoiseType::White:
        registerBlockProcessor(&NoiseGenerator::processBlockWhite);
        break;
    case NoiseType::Pink:
        registerBlockProcessor(&NoiseGenerator::processBlockPink);
        break;
    default:
        break;
    }
}

void NoiseGenerator::processBlockWhite()
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Generate white noise sample in range [-1.0, 1.0]
        host_float white = rand.nextRandomSample();
        outputBus->l[i] = white;
        outputBus->r[i] = white;
    }
}

void NoiseGenerator::processBlockPink()
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Generate white noise sample in range [-1.0, 1.0]
        host_float white = rand.nextRandomSample();

        // --- Paul Kellet's Pink Noise Filter ---
        // Each term simulates a low-pass filter with different time constants
        // The coefficients are empirically derived for pink noise behavior
        b0 = 0.99886 * b0 + white * 0.0555179;
        b1 = 0.99332 * b1 + white * 0.0750759;
        b2 = 0.96900 * b2 + white * 0.1538520;
        b3 = 0.86650 * b3 + white * 0.3104856;
        b4 = 0.55000 * b4 + white * 0.5329522;
        b5 = -0.7616 * b5 - white * 0.0168980;
        host_float pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362;
        b6 = white * 0.115926;

        // Scale output to normalize level (empirical factor)
        outputBus->l[i] = pink * 0.11;
        outputBus->r[i] = pink * 0.11;
    }
}

void NoiseGenerator::processBlockWhite(DSPObject *dsp)
{
    NoiseGenerator *self = static_cast<NoiseGenerator *>(dsp);
    self->processBlockWhite();
}

void NoiseGenerator::processBlockPink(DSPObject *dsp)
{
    NoiseGenerator *self = static_cast<NoiseGenerator *>(dsp);
    self->processBlockPink();
}