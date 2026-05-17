#pragma once

#include <game/scene/scene.h>

#include <engine/window/window.h>
#include <engine/graphics/shader.h>
#include <engine/ui/ui_manager.h>
#include <engine/map/map_loader.h>
#include <engine/physics/aabb.h>

#include <game/ui/playerHUD.h>
#include <game/player/player.h>
#include <game/core/entity.h>

#include <string>
#include <vector>
using std::string, std::vector;

class LevelScene : public Scene
{
public:
    LevelScene(const string &map_file, Window *game_window, UIManager *ui);
    virtual ~LevelScene() = default;

    void Init(SceneManager *manager) override;
    void ProcessInput() override;
    void Update(f32 dt) override;
    void Render(f32 dt) override;
    void Cleanup() override;

private:
    string map_filepath;
    Window *window;
    UIManager *ui_manager;

    Player *player;
    Shader *main_shader;
    LoadMap current_map;
    PlayerHUD player_hud;

    vector<AABB> current_frame_colliders;
    vector<AABB> level_colliders;
    vector<Entity *> entities;
};