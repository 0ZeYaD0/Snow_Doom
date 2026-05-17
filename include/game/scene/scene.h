#pragma once

#include <engine/core/defines.h>

class SceneManager;

class Scene
{
public:
    virtual void Init(SceneManager *manager) = 0;
    virtual void ProcessInput() = 0;
    virtual void Update(f32 dt) = 0;
    virtual void Render(f32 dt) = 0;
    virtual void Cleanup() = 0;

    virtual ~Scene() = default;

protected:
    SceneManager *scene_manager = nullptr;
};