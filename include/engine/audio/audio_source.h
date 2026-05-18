#pragma once

#include <engine/core/defines.h>
#include <engine/audio/audio_buffer.h>

#include <OpenAL/al.h>
#include <glm/glm.hpp>

class AudioSource
{
public:
    AudioSource();
    ~AudioSource();

    void Play(AudioBuffer *buffer);
    void SetPosition(glm::vec3 pos);
    void SetLooping(bool loop);
    void SetSpatial(bool spatial);
    void SetVolume(f32 volume);
    f32 GetVolume() const;
    void Stop();

private:
    u32 source_id;
};