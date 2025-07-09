#pragma once
#include <memory>

// Structure representing a managed voice with metadata
template <typename TVoice>
struct ManagedVoice
{
    std::unique_ptr<TVoice> voice; // Owned voice instance
    int age = 0;                   // Allocation age counter
    int note = -1;                 // Last assigned MIDI note
    bool reclaimable = true;       // Can be reused if needed (e.g., after Note-Off)
};
