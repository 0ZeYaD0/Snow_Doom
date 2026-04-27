#pragma once

#include <engine/core/defines.h>

#include <engine/window/window.h>
#include <game/player/camera.h>
#include <engine/graphics/shader.h>
#include <game/entities/entity.h>

#include <vector>
using std::vector;

class Game
{
public:
    Game();
    ~Game();

    inline bool WindowShouldClose() { return window.WindowShouldClose(); }

    void Run();
    void Exit();

private:
    void ProcessInput();
    void Update();
    void Render();

    Camera cam;
    Window window;

    f32 delta_time;
    f32 last_frame;

    Shader *main_shader;

    vector<Mesh> loaded_meshes;
    vector<Entity> entities;
};