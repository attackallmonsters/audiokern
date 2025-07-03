#include "NoiseGenerator.h"
#include <chrono> // Used to seed the RNG with high-resolution time
#include "dsp_types.h"

// Constructor: seeds RNG and sets noise type
NoiseGenerator::NoiseGenerator()
{
    registerSampleGenerator(&NoiseGenerator::generateSample);

    // WHite noise is default
    setType(NoiseType::White);

    // Seed RNG using the system clock to ensure randomness across runs
    rng.seed(static_cast<unsigned int>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()));
}

// Allows dynamic switching between white and pink noise
void NoiseGenerator::setType(NoiseType type)
{
    noiseType = type;
}

void NoiseGenerator::generateSample(
    Oscillator *osc,
    const dsp_float & /*frequency*/,
    const dsp_float & /*phase*/,
    dsp_float &left,
    dsp_float &right,
    const dsp_float & /*modLeft*/,
    const dsp_float & /*modRight*/)
{
    NoiseGenerator *noise = static_cast<NoiseGenerator *>(osc);

    // Generate white noise sample in range [-1.0, 1.0]
    dsp_float white = noise->dist(noise->rng);

    // If white noise is selected, return it directly
    if (noise->noiseType == NoiseType::White)
    {
        left = right = white;
        return;
    }

    // --- Paul Kellet's Pink Noise Filter ---
    // Each term simulates a low-pass filter with different time constants
    // The coefficients are empirically derived for pink noise behavior
    noise->b0 = 0.99886 * noise->b0 + white * 0.0555179;
    noise->b1 = 0.99332 * noise->b1 + white * 0.0750759;
    noise->b2 = 0.96900 * noise->b2 + white * 0.1538520;
    noise->b3 = 0.86650 * noise->b3 + white * 0.3104856;
    noise->b4 = 0.55000 * noise->b4 + white * 0.5329522;
    noise->b5 = -0.7616 * noise->b5 - white * 0.0168980;
    dsp_float pink = noise->b0 + noise->b1 + noise->b2 + noise->b3 + noise->b4 + noise->b5 + noise->b6 + white * 0.5362;
    noise->b6 = white * 0.115926;

    // Scale output to normalize level (empirical factor)
    left = right = pink * 0.11;
}
