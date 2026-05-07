#pragma once

#include <engine/core/defines.h>

#include <string>
using std::string;

class Texture
{
public:
    u32 ID;
    i32 width, height, channels;

    Texture(const string &filepath);
    ~Texture();

    void Bind(u32 slot = 0) const;
    void Unbind() const;
};