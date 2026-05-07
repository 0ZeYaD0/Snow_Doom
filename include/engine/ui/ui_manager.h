#pragma once

#include <engine/graphics/shader.h>
#include <engine/window/window.h>
#include <game/player/player.h>
#include <engine/graphics/texture.h>

class UIManager
{
public:
    UIManager();
    ~UIManager();

    void Init();
    void Render(const Window &window, const Player *player);
    void DrawSprite(Texture* texture, glm::vec2 position, glm::vec2 size, glm::vec4 tint);
    void DrawSpriteFrame(Texture* texture, glm::vec2 position, glm::vec2 size, int current_frame, int total_frames, glm::vec4 tint);

private:
    void DashCounterUI(const Window &window, const Player *player);
    void DrawGun(const Window &window, const Player *player);
    
private:
    Shader *ui_shader;
    u32 VAO, VBO;
};