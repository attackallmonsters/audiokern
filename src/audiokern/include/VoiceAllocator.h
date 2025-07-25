#pragma once
#include <vector>
#include <memory>

/**
 * @brief Container for a single voice and its state metadata.
 *
 * This structure wraps a voice instance (`TVoice`) and tracks relevant voice
 * allocation metadata:
 * - The `age` counter is incremented on each allocation to track reuse order.
 * - The `note` represents the last MIDI note assigned to this voice.
 * - The `reclaimable` flag determines if the voice may be reused (e.g. on Note-Off).
 *
 * This structure is intended for use with `VoiceAllocator`.
 */
template <typename TVoice>
struct ManagedVoice
{
    std::unique_ptr<TVoice> voice; ///< Owned voice instance
    int age = 0;                   ///< Allocation age counter (used for oldest-first logic)
    int note = -1;                 ///< Last MIDI note assigned to this voice
    bool reclaimable = true;      ///< Whether this voice can be reused (e.g., after Note-Off)
};

/**
 * @brief Generic voice pool manager for polyphonic synthesizers.
 *
 * `VoiceAllocator` manages a list of polyphonic voice objects and provides logic
 * for note-to-voice assignment, reuse, and access. Voices are wrapped in `ManagedVoice`
 * structures which track MIDI note assignment, allocation age, and reclaimability.
 *
 * ### Usage Example
 * @code
 * VoiceAllocator<JPVoice> allocator;
 * allocator.add(std::make_unique<JPVoice>());
 * allocator.add(std::make_unique<JPVoice>());
 *
 * JPVoice* voice = allocator.allocate(note);
 * voice->trigger(note, velocity);
 *
 * auto all = allocator.getVoices();
 * for (auto* v : all) v->render();
 * @endcode
 *
 * @tparam TVoice Any voice type implementing your DSP voice logic (e.g., `JPVoice`)
 */
template <typename TVoice>
class VoiceAllocator
{
public:
    /**
     * @brief Clears the internal voice list and resets all metadata.
     */
    void clear();

    /**
     * @brief Adds a new voice to the pool.
     *
     * Takes ownership of the voice instance.
     *
     * @param v A unique pointer to a TVoice instance.
     */
    void add(std::unique_ptr<TVoice> v);

    /**
     * @brief Returns raw pointers to all managed voices.
     *
     * @return A vector of TVoice* (non-owning).
     */
    std::vector<TVoice*> getVoices();

    /**
     * @brief Allocates a voice for the given MIDI note.
     *
     * Reuses the oldest reclaimable voice, if available. If none is reclaimable,
     * it reuses the oldest voice regardless of state.
     *
     * @param note MIDI note to assign.
     * @return A pointer to the selected TVoice.
     */
    TVoice* allocate(int note);

    /**
     * @brief Returns the voice currently assigned to the given MIDI note.
     *
     * If no voice is found, returns nullptr.
     *
     * @param note MIDI note number.
     * @return A pointer to the matching TVoice, or nullptr.
     */
    TVoice* select(int note);

    /**
     * @brief Marks a voice with a given note as reclaimable (e.g. after Note-Off).
     *
     * @param note MIDI note number.
     */
    void setReclaimable(int note);

    /**
     * @brief Accesses a voice by index.
     *
     * @param index Index of the voice.
     * @return Pointer to the TVoice.
     */
    TVoice* getVoice(size_t index);

    /**
     * @brief Const version of indexed voice access.
     *
     * @param index Index of the voice.
     * @return Const pointer to the TVoice.
     */
    const TVoice* getVoice(size_t index) const;

    /**
     * @brief Returns the MIDI note currently assigned to a voice.
     *
     * @param index Index of the voice.
     * @return MIDI note value (or -1 if unassigned).
     */
    int getNote(int index) const;

    /**
     * @brief Applies a function to all voices.
     *
     * @tparam Func A callable taking `TVoice&` or `auto&`
     * @param fn Function to apply to each voice.
     */
    template <typename Func>
    void forEachVoice(Func&& fn);

private:
    std::vector<ManagedVoice<TVoice>> voices; ///< Internal list of managed voices
};

// Implementation
#include "VoiceAllocator.tpp"
