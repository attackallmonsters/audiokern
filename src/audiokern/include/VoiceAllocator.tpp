#pragma once
#include <stdexcept>

template <typename TVoice>
void VoiceAllocator<TVoice>::clear()
{
    voices.clear();
}

template <typename TVoice>
void VoiceAllocator<TVoice>::add(std::unique_ptr<TVoice> v)
{
    ManagedVoice<TVoice> mv;
    mv.voice = std::move(v);
    mv.age = 0;
    mv.note = -1;
    mv.reclaimable = true;
    voices.push_back(std::move(mv));
}

template <typename TVoice>
std::vector<TVoice *> VoiceAllocator<TVoice>::getVoices()
{
    std::vector<TVoice *> result;
    result.reserve(voices.size());
    for (auto &mv : voices)
    {
        result.push_back(mv.voice.get());
    }
    return result;
}

template <typename TVoice>
TVoice *VoiceAllocator<TVoice>::allocate(int note)
{
    if (voices.empty())
        PANIC("VoiceAllocator: No voices available");

    int bestIndex = -1;
    int bestAge = -1;

    // Step 1: Prefer reclaimable voices
    for (size_t i = 0; i < voices.size(); ++i)
    {
        if (voices[i].reclaimable && voices[i].age > bestAge)
        {
            bestAge = voices[i].age;
            bestIndex = static_cast<int>(i);
        }
    }

    // Step 2: If no reclaimable voice, pick oldest overall
    if (bestIndex == -1)
    {
        for (size_t i = 0; i < voices.size(); ++i)
        {
            if (voices[i].age > bestAge)
            {
                bestAge = voices[i].age;
                bestIndex = static_cast<int>(i);
            }
        }
    }

    // Update ages
    for (auto &mv : voices)
        mv.age++;

    auto &selected = voices[bestIndex];

    selected.age = 0;
    selected.reclaimable = false;
    selected.note = note;

    return selected.voice.get();
}

template <typename TVoice>
TVoice *VoiceAllocator<TVoice>::select(int note)
{
    for (auto &mv : voices)
    {
        if (mv.note == note)
            return mv.voice.get();
    }

    return nullptr;
}

template <typename TVoice>
void VoiceAllocator<TVoice>::setReclaimable(int note)
{
    for (auto &mv : voices)
    {
        if (mv.note == note)
        {
            mv.reclaimable = true;
            mv.note = -1;
            break;
        }
    }
}

template <typename TVoice>
TVoice *VoiceAllocator<TVoice>::getVoice(size_t index)
{
    return voices.at(index).voice.get();
}

template <typename TVoice>
const TVoice *VoiceAllocator<TVoice>::getVoice(size_t index) const
{
    return voices.at(index).voice.get();
}

template <typename TVoice>
int VoiceAllocator<TVoice>::getNote(int index) const
{
    return voices[index].note;
}

template <typename TVoice>
template <typename Func>
void VoiceAllocator<TVoice>::forEachVoice(Func &&fn)
{
    for (auto &managed : voices)
    {
        if (managed.voice)
            fn(*managed.voice);
    }
}