#pragma once

#include <vector>
#include <memory>
#include "MidiProcessor.h"
#include <climits>

enum class VoiceStatus
{
    stopped,
    active 
};

// Internal wrapper per voice
template <typename TVoice>
struct ManagedVoice
{
    std::unique_ptr<TVoice> voice;
    VoiceStatus status = VoiceStatus::stopped;
    int note = -1; // Assigned MIDI note
};

template <typename TVoice, typename TDerived>
class VoiceManager
{
public:
    // Ctor
    VoiceManager() = default;

    // Initializes the instance
    void initialize();

    // Sets the number of voices
    void setNumVoices(size_t num);

    // Handles a note in event
    void handleNote(int note, host_float velocity);

    // Template for sending various method calls to all voices
    template <typename Method, typename... Args>
    void toAll(Method method, Args &&...args);

    // MIDI and tuning options
    MidiProcessor midiProcessor;

private:
    ManagedVoice<TVoice>* getNextVoice();
    ManagedVoice<TVoice>* currentVoice;

    std::vector<ManagedVoice<TVoice>> voices;
    int nextVoiceIndex;
};

#include "VoiceManager.tpp"
