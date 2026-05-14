#include <engine/audio/audio_buffer.h>

#include <stb/stb_vorpis.h>

AudioBuffer::AudioBuffer(const string &filepath)
{
    alGenBuffers(1, &buffer_id);
    LoadOgg(filepath);
}

AudioBuffer::~AudioBuffer()
{
    alDeleteBuffers(1, &buffer_id);
}

void AudioBuffer::LoadOgg(const string filepath)
{
    i32 channels, sample_rate;
    i16 *decoded_data;

    i32 samples = stb_vorbis_decode_filename(
        filepath.c_str(), &channels, &sample_rate, &decoded_data
    );

    if(samples == -1)
    {
        std::cerr << "failed to load OGG Audio File: " << filepath << std::endl;
        return;
    }

    ALenum format;
    if(channels == 1) {
        format = AL_FORMAT_MONO16;
    }
    else if(channels == 2) {
        format = AL_FORMAT_STEREO16;
    }
    else {
        std::cerr << "unsupported channel count in: " << filepath << std::endl;
        free(decoded_data);
        return;
    }

    ALsizei data_size = samples * channels * sizeof(short);
    alBufferData(buffer_id, format, decoded_data, data_size, sample_rate);
    free(decoded_data);
}