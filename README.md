![](audiokern.jpg)
# audiokern

**audiokern** is a minimalistic DSP framework designed with simplicity and modularity in mind.

The name combines _“audio”_ and the German word _“Kern”_ (meaning “core”) – a nod to its purpose as the **core layer of sound synthesis and signal processing**.  
It aims to be lightweight, readable, and host-independent.

## Purpose

This project is being developed as a personal learning environment and may evolve into a flexible base for audio plugin development.  
The focus is not on performance or feature-completeness, but rather on:

- **Ease of use** and a clear, consistent API
- **Modular building blocks** for DSP: oscillators, envelopes, filters, LFOs, etc.
- **Minimal external dependencies** – just C++
- **Readable and well-commented code** to support learning and extension

## Disclaimer

> I am still a **beginner in both C++ and DSP development**.  
> This codebase is in active development, and things may break, change or improve quickly.  
> Feedback, suggestions or contributions are welcome!

## Structure

- `src/dsp`     – core signal processing modules
- `src/adsr`    – ADSR PD external => https://github.com/attackallmonsters/audiokern/tree/main/bin/adsr
- `src/LFO`     – LFO PD external => https://github.com/attackallmonsters/audiokern/tree/main/bin/lfo
- `src/jpsynt` – JHP Synth PD external (inspiered by the Roland JP-8000) => https://github.com/attackallmonsters/audiokern/tree/main/bin/jpsynth
- `obj/`, `lib/` – build artifacts (ignored via `.gitignore`)

## Build Instructions

### Prerequisites

- **Linux system** (x86_64 or armv7l)
- **g++**, **make**
- Optional: Pure Data (`pd`) for testing Pd externals

### Building (x86 or ARM)
Each src directory contains a Makefile and
- `./md.sh`
- `./mr.sh`

These are only shortcuts for 
- `make debug`
- `make release`

The library is copied directly into the bin folder for the respective platform

- Linux x64
- Linux ARM
- Windows (currently not working)

There you will find some Pure Data patches for testing as well.

I'll try to keep the build functional in the main branch. But I can't promise it.

### Organelle Compatibility
The framework is designed to work out of the box with the **Critter & Guitari Organelle M / S**, which runs a Raspberry Pi 3–based Linux system.

No changes to the Patch Manager or system configuration are needed – simply copy the compiled `.pd_linux` files into your patch directory.

> **Important:** The Organelle uses **GCC 4.9.2**, which lacks full C++17 support.  
> The code is kept compatible by avoiding newer language features. When targeting the Organelle, always build with the correct toolchain.

### Organelle Compatibility
The framework is designed to work out of the box with the **Critter & Guitari Organelle M / S**, which runs a Raspberry Pi 3–based Linux system.

No changes to the Patch Manager or system configuration are needed – simply copy the compiled `.pd_linux` files into your patch directory.

> ⚠️ **Important:** The Organelle uses **GCC 4.9.2**, which lacks full C++17 support.  
> The code is kept compatible by avoiding newer language features. When targeting the Organelle, always build with the correct toolchain.

---