#pragma once

#include <engine/core/defines.h>

#include <OpenAL/al.h>
#include <iostream>
#include <string>
using std::string;

class AudioBuffer
{
public:
    AudioBuffer(const string &filepath);
    ~AudioBuffer();

    u32 GetID() const { return buffer_id; }

private:
    u32 buffer_id;

    void LoadOgg(const string filepath);
};