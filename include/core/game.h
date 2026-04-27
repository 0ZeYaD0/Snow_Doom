#pragma once

#include <defines.h>

#include <window/window.h>
#include <player/camera.h>
#include <graphics/shader.h>
#include <entities/entity.h>

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