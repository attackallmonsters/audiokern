#pragma once
#include <vector>

// Structure representing a managed voice with metadata
template <typename TVoice>
struct ManagedVoice
{
    std::unique_ptr<TVoice> voice; // Owned voice instance
    int age = 0;                   // Allocation age counter
    int note = -1;                 // Last assigned MIDI note
    bool reclaimable = true;       // Can be reused if needed (e.g., after Note-Off)
};


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

    // 
    template <typename Func>
    void forEachVoice(Func&& fn);

private:
    std::vector<ManagedVoice<TVoice>> voices;
};

#include "VoiceAllocator.tpp"
