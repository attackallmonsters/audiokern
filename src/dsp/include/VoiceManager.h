#pragma once

#include <vector>
#include <memory>
#include "MidiProcessor.h"
#include "TunigSystem.h"
#include "VoiceAllocator.h"
#include <climits>

template <typename TVoice, typename TDerived>
class VoiceManager
{
public:
    // Ctor
    VoiceManager();

    // Dtor
    virtual ~VoiceManager();

    // Initializes the instance
    void initialize();

    // Sets the number of voices
    void setNumVoices(size_t num);

    // Handles a note in event
    void handleNote(int note, host_float velocity);

    // Abstract member for voice instanciation
    virtual std::unique_ptr<TVoice> createVoice() = 0;

    // Note on event handeld by derived implementation
    virtual void noteOn(TVoice *voice, int note) = 0;

    // Note off event handeld by derived implementation
    virtual void noteOff(TVoice *voice) = 0;

    // MIDI processing
    MidiProcessor midiProcessor;

    // The tuning system
    TunigSystem tuningSystem;

    // Voice allocation mangement, allocator contains global voice data
    VoiceAllocator<TVoice> allocator;
};

#include "VoiceManager.tpp"
