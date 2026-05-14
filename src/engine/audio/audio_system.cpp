#include <engine/audio/audio_system.h>

ALCdevice *AudioSystem::device = nullptr;
ALCcontext *AudioSystem::context = nullptr;

void AudioSystem::Init()
{
    device = alcOpenDevice(nullptr);
    if(!device)
    {
        std::cerr << "Failed to open OpenAL Audio Device" << std::endl;
        return;
    }

    context = alcCreateContext(device, nullptr);
    if(!context)
    {
        std::cerr << "failed to create OpenAL Context..." << std::endl;
        return;
    }

    if(!alcMakeContextCurrent(context))
        std::cerr << "failed to make OpenAL context current..." << std::endl;
}

void AudioSystem::Cleanup()
{
    alcMakeContextCurrent(nullptr);
    
    if(context) alcDestroyContext(context);
    if(device) alcCloseDevice(device);
}