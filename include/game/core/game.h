#pragma once

#include <engine/core/defines.h>

#include <engine/window/window.h>
#include <game/player/camera.h>
#include <engine/graphics/shader.h>
#include <game/entities/player.h>
#include <engine/physics/aabb.h>

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

    Player *player;
    vector<AABB> level_colliders;
    Window window;

    f32 delta_time;
    f32 last_frame;

    Shader *main_shader;

    vector<Mesh> loaded_meshes;
    vector<Mesh> floor_meshes;
    vector<Entity> entities;
};