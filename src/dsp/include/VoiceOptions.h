#pragma once

// Contains options for oscillation configuration

// OScillator types for oscillator 1
enum class CarrierOscillatiorType
{
    Saw,       // Saw oscillator
    Square,    // Sqare oscillator
    Triangle,  // Triangle oscillator
    Sine,      // Sine oscillator
    Cluster,   // Harmonioc numbers cluster oscillator
    Fibonacci, // Fibonacci number oscillator
    Mirror,    // Mirrored signal oscillator
    Modulo     // Modulo wave ocscillator
};

// OScillator types for oscillator 1
enum class ModulatorOscillatorType
{
    Saw,       // Saw oscillator
    Square,    // Square oscillator
    Triangle,  // Triangle oscsillator
    Sine,      // Sine oscillator
    Cluster,   // Harmonioc numbers cluster oscillator
    Fibonacci, // Fibonacci number oscillator
    Mirror,    // Mirrored signal oscillator
    Modulo,    // Modulo wave ocscillator
    Bit        // Bitcrusher oscillator
};

// Defines the available types of noise
enum class NoiseType
{
    White, // Uncorrelated white noise
    Pink   // Filtered pink noise (equal energy per octave)
};

// FM operations modes
enum class FMType
{
    None,
    Linear,
    ThroughZero,
    Relative
};

// Enum for selecting the filter mode
enum class FilterMode
{
    LPF12,
    HPF12,
    BPF12
};