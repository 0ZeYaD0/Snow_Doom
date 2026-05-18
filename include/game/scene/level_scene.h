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

#include <functional>
#include <string>
#include <vector>
using std::string, std::vector, std::function;

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
    void Reset();

    inline void SetSoundtrack(string st) {
        soundtrack_file = st;
    }

    void SetCompletionLogic(function<bool()> condition, function<void()> callback);

    Player* GetPlayer() const { return player; }

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

    string soundtrack_file, you_died_file;

    AudioSource you_died_source;
    AudioBuffer *you_died_music = nullptr;

    AudioSource bg_audio_source;
    AudioBuffer* bg_music = nullptr;

    function<bool()> check_completion;
    function<void()> on_level_complete;
    bool is_completed = false;
    bool played_death_sound = false;
};