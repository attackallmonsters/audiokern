template <typename TVoice, typename TDerived>
VoiceManager<TVoice, TDerived>::VoiceManager()
{
}

template <typename TVoice, typename TDerived>
VoiceManager<TVoice, TDerived>::~VoiceManager()
{
    
}

template <typename TVoice, typename TDerived>
void VoiceManager<TVoice, TDerived>::initialize()
{
    midiProcessor.initialize();
    tuningSystem.initialize();
}

template <typename TVoice, typename TDerived>
void VoiceManager<TVoice, TDerived>::setNumVoices(size_t num)
{
    allocator.clear();

    for (size_t i = 0; i < num; ++i)
    {
        // Create voice via factory method (returns std::unique_ptr<TVoice>)
        std::unique_ptr<TVoice> voice = static_cast<TDerived*>(this)->createVoice();

        // Transfer ownership to allocator
        allocator.add(std::move(voice));
    }
}

template <typename TVoice, typename TDerived>
void VoiceManager<TVoice, TDerived>::handleNote(int note, host_float velocity)
{
    if (velocity > 0)
    {
        TVoice* v = allocator.allocate(note);
        static_cast<TDerived*>(this)->noteOn(v, note);
    }
    else
    {
        TVoice* v = allocator.select(note);
        static_cast<TDerived*>(this)->noteOff(v);
        allocator.setReclaimable(note);
    }
        
}