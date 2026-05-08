#pragma once

#include <engine/graphics/shader.h>
#include <engine/window/window.h>
#include <engine/graphics/texture.h>

class UIManager
{
public:
    UIManager();
    ~UIManager();

    void Init();

    // --- State Management ---
    void Begin(const Window &window);
    void End();

    // --- Generic Drawing Primitives ---
    void DrawSprite(Texture *texture, glm::vec2 position, glm::vec2 size, f32 rotation = 0.0f, glm::vec4 tint = glm::vec4(1.0f));
    void DrawSpriteFrame(Texture *texture, glm::vec2 position, glm::vec2 size, int current_frame, int total_frames, glm::vec4 tint = glm::vec4(1.0f));
    void DrawRect(glm::vec2 position, glm::vec2 size, glm::vec4 color, glm::vec4 bg_color, f32 fill_amount, f32 rotation = 0.0f);

private:
    Shader *ui_shader;
    unsigned int VAO, VBO;
};
