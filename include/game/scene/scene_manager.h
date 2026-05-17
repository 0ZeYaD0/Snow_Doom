#pragma once

#include <engine/core/defines.h>
#include <game/scene/scene.h>

class SceneManager
{
public:
    SceneManager() = default;
    ~SceneManager();

    void ChangeScene(Scene *new_scene);

    void ProcessInput();
    void Update(f32 dt);
    void Render(f32 dt);

private:
    Scene *current_scene = nullptr;
    Scene *next_scene = nullptr;
};