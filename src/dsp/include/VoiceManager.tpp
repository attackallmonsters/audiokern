
template <typename TVoice, typename TDerived>
void VoiceManager<TVoice, TDerived>::initialize()
{
    nextVoiceIndex = 0;
}

template <typename TVoice, typename TDerived>
void VoiceManager<TVoice, TDerived>::setNumVoices(size_t num)
{
    voices.resize(num);
    
    for (auto &v : voices)
    {
        if (!v.voice)
            v.voice = std::make_unique<TVoice>();

        v.status = VoiceStatus::stopped;
        v.note = -1;
    }
}

template <typename TVoice, typename TDerived>
ManagedVoice<TVoice>* VoiceManager<TVoice, TDerived>::getNextVoice() 
{
    ManagedVoice<TVoice>* v = &voices[nextVoiceIndex];
    nextVoiceIndex = (nextVoiceIndex + 1) % voices.size();
    return v;
}

template <typename TVoice, typename TDerived>
void VoiceManager<TVoice, TDerived>::handleNote(int note, host_float velocity)
{
    currentVoice = getNextVoice();
}


// ===============================================


template<typename TVoice, typename TDerived>
template<typename Method, typename... Args>
void VoiceManager<TVoice, TDerived>::toAll(Method method, Args&&... args)
{
    for (auto& v : voices)
    {
        (v.voice.get()->*method)(std::forward<Args>(args)...);
    }
}
