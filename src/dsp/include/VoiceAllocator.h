#pragma once
#include <vector>
#include "ManagedVoice.h"

// Allocates and tracks voices for polyphonic playback
template <typename TVoice>
class VoiceAllocator
{
public:
    // Clears all managed voices
    void clear();

    // Adds a new voice (takes ownership)
    void add(std::unique_ptr<TVoice> v);

    // Returns raw pointers to all voices
    std::vector<TVoice *> getVoices();

    // Returns a reference to a suitable voice (preferably reclaimable)
    TVoice* allocate(int note);

    // Returns the voice currently assigned to the given note, or nullptr
    TVoice* select(int note);

    // Sets a note to reclaimable
    void setReclaimable(int note);

    // Index based access
    TVoice* getVoice(size_t index);
    const TVoice* getVoice(size_t index) const;

    // Return the note of a voice
    int getNote(int index) const;

private:
    std::vector<ManagedVoice<TVoice>> voices;
};

#include "VoiceAllocator.tpp"
