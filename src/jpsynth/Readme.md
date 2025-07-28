![](jpsynth.jpg)

# JP Synth

**JP Synth** is a polyphonic virtual analog synthesizer inspired by the iconic Roland synth architecture of the late '90s. Built entirely in C++, it features a modular DSP design optimized for real-time audio performance on desktop and embedded platforms.

## 🎛 Features

- **Supersaw oscillator** with up to 9 detuned voices, stereo spread & phase modulation
- **Wavetable synthesis** with pluggable, band-limited tables and optional FM routing
- **Analog-style filter** (`KorgonFilter`) with nonlinear feedback (LP / HP modes)
- **Nonlinear ADSR envelope** with retrigger and optional smooth start
- **Flexible LFOs** with multiple waveforms, smoothing, phase reset detection, and modulation outputs
- **Effects section** featuring block-based **Comb Delay**, **Ping-Pong routing**, and **Nebular Reverb** (FDN-based with feedback matrix)
- **Voice allocator** using age-based replacement without manual idle tracking
- **Multithreaded architecture** with scalable thread pool for efficient voice and effect processing
- Fully **Organelle-compatible** (Pure Data `.pd_linux` external, compiles on older GCC)

## ⚙️ Build Instructions

### Requirements

- A modern C++17-capable compiler (GCC ≥ 7 or Clang ≥ 5)
- GNU Make
- Pure Data SDK (for building the `.pd_linux` external)
- Optional: Organelle cross-toolchain for embedded builds

### Build Targets

```bash
./md.sh -d        # Builds core library + Pure Data external for debug
./mr.sh -d        # Builds core library + Pure Data external for release