#include "FibonacciWavetable.h"
#include <cmath>

static bool isPerfectSquare(int x)
{
    int s = static_cast<int>(std::sqrt(x));
    return s * s == x;
}

static bool isFibonacci(int n)
{
    // A number is Fibonacci, when 5*n² + 4 or  5*n² - 4 is square
    int test1 = 5 * n * n + 4;
    int test2 = 5 * n * n - 4;
    return isPerfectSquare(test1) || isPerfectSquare(test2);
}

// Amplitude function for WaveGenerator
static dsp_float fibonacciAmplitude(int harmonic)
{
    if (isFibonacci(harmonic))
        return 1.0 / harmonic;
    else
        return 0.0;
}

void FibonacciWavetable::createWavetable(DSPBuffer &buffer, dsp_float frequency)
{
    // Fill one full waveform cycle (0 to 2π) across the buffer
    WaveformGenerator::generateWavetable(buffer, frequency, fibonacciAmplitude, 0.5);
}
