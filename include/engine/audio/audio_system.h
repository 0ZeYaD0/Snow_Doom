#pragma once

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include <iostream>

class AudioSystem
{
public:
    static void Init();
    static void Cleanup();

private:
    static ALCdevice *device;
    static ALCcontext *context;
};