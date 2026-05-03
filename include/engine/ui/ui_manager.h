#pragma once

#include <engine/graphics/shader.h>
#include <engine/window/window.h>
#include <game/player/player.h>

class UIManager
{
public:
    UIManager();
    ~UIManager();

    void Init();
    void Render(const Window &window, const Player *player);

private:
    void DashCounterUI(const Window &window, const Player *player);

private:
    Shader *ui_shader;
    u32 VAO, VBO;
};