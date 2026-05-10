#pragma once

#include <engine/core/defines.h>

#include <engine/window/window.h>
#include <game/player/camera.h>
#include <engine/graphics/shader.h>
#include <game/player/player.h>
#include <engine/physics/aabb.h>
#include <engine/ui/ui_manager.h>
#include <game/ui/playerHUD.h>
#include <engine/map/map_loader.h>

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
    LoadMap current_map;

    vector<Mesh> loaded_meshes;
    vector<Mesh> floor_meshes;
    vector<Entity *> entities;

    UIManager ui_manager;
    PlayerHUD player_hud;
};