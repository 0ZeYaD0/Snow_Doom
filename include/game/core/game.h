#pragma once

#include <engine/core/defines.h>
#include <engine/window/window.h>
#include <engine/ui/ui_manager.h>
#include <game/scene/scene_manager.h>

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

    Window window;
    UIManager ui_manager;
    SceneManager scene_manager;

    f32 delta_time;
    f32 last_frame;
};