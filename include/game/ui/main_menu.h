#pragma once

#include <game/scene/scene.h>

#include <engine/window/window.h>
#include <engine/ui/ui_manager.h>
#include <engine/graphics/texture.h>
#include <engine/map/map_loader.h>

#include <glm/glm.hpp>

class MainMenuScene : public Scene
{
public:
    MainMenuScene(Window *game_window, UIManager *ui);
    virtual ~MainMenuScene() = default;

    void Init(SceneManager* manager) override;
    void ProcessInput() override;
    void Update(f32 dt) override;
    void Render(f32 dt) override;
    void Cleanup() override;

private:
    Window *window;
    UIManager *ui_manager;

    Texture *title_tex;
    Texture *play_btn_tex;
    Texture *quit_btn_tex;

    glm::vec4 play_btn_bounds;
    glm::vec4 quit_btn_bounds;

    bool CheckButtonClick(glm::vec4 bounds, f32 mouse_x, f32 mouse_y);

    Shader* bg_shader;
    LoadMap bg_map;
    f32 camera_angle;

    AudioSource audio_source;
    AudioBuffer* soundtrack = nullptr;
};