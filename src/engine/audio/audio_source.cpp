#include <engine/audio/audio_source.h>

AudioSource::AudioSource()
{
    alGenSources(1, &source_id);

    // default vals
    alSourcef(source_id, AL_PITCH, 1.0f);
    alSourcef(source_id, AL_GAIN, 1.0f);
    alSource3f(source_id, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(source_id, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(source_id, AL_LOOPING, AL_FALSE);
}

AudioSource::~AudioSource() {
    alDeleteSources(1, &source_id);
}

void AudioSource::Play(AudioBuffer* buffer) {
    if (!buffer) return;
    
    alSourcei(source_id, AL_BUFFER, buffer->GetID());
    alSourcePlay(source_id);
}

void AudioSource::SetPosition(glm::vec3 pos)
{
    alSource3f(source_id, AL_POSITION, pos.x, pos.y, pos.z);
}