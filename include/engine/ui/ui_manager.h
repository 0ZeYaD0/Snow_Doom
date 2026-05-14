#pragma once

#include <engine/graphics/shader.h>
#include <engine/window/window.h>
#include <engine/graphics/texture.h>

#include <vector>
#include <array>
using std::vector, std::array;

struct UIVertex
{
    glm::vec2 position;
    glm::vec2 uv;
    glm::vec4 color;
    f32 is_textured;
};

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
    void Flush();
    void StartBatch();
    void CheckFlushCondition(Texture *next_texture);

    Shader *ui_shader;
    u32 VAO, VBO, EBO;

    static const u64 MAX_QUAD_COUNT   = 1000;
    static const u64 MAX_VERTEX_COUNT = MAX_QUAD_COUNT * 4;
    static const u64 MAX_INDEX_COUNT  = MAX_QUAD_COUNT * 6;

    UIVertex *quad_buffer;
    UIVertex *quad_buffer_ptr;
    u32 index_count;

    Texture *current_texture;
};
