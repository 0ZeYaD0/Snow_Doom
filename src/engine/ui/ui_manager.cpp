#include <engine/ui/ui_manager.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

UIManager::UIManager()
    :   ui_shader(nullptr), VAO(0), VBO(0), EBO(0),
        quad_buffer(nullptr), quad_buffer_ptr(nullptr),
        index_count(0), current_texture(nullptr)
{
    quad_buffer = new UIVertex[MAX_VERTEX_COUNT];
}

UIManager::~UIManager()
{
    if (ui_shader)
        delete ui_shader;

    delete[] quad_buffer;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void UIManager::Init()
{
    ui_shader = new Shader("res/shaders/ui_vertex.glsl", "res/shaders/ui_fragment.glsl");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_COUNT * sizeof(UIVertex), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const void*)offsetof(UIVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const void*)offsetof(UIVertex, uv));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const void*)offsetof(UIVertex, color));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const void*)offsetof(UIVertex, is_textured));

    u32 indices[MAX_INDEX_COUNT];
    u32 offset = 0;

    for (u64 i = 0; i < MAX_INDEX_COUNT; i += 6) {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;
        offset += 4;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void UIManager::Begin(const Window &window)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ui_shader->Use();
    glm::mat4 projection = glm::ortho(0.0f, (f32)window.GetWidth(), (f32)window.GetHeight(), 0.0f);
    ui_shader->SetMat4("projection", projection);

    StartBatch();
}

void UIManager::End()
{
    Flush();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void UIManager::StartBatch()
{
    quad_buffer_ptr = quad_buffer;
    index_count = 0;
}

void UIManager::Flush()
{
    if(index_count == 0) return;

    u32 data_sz = (u32)((uint8_t*)quad_buffer_ptr - (uint8_t*)quad_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data_sz, quad_buffer);

    if (current_texture) {
        current_texture->Bind(0);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    
    if (current_texture) {
        current_texture->Unbind();
    }
}

void UIManager::CheckFlushCondition(Texture* next_texture)
{
    if (index_count >= MAX_INDEX_COUNT || current_texture != next_texture) {
        Flush();
        StartBatch();
        current_texture = next_texture;
    }
}

void UIManager::DrawSprite(Texture* texture, glm::vec2 position, glm::vec2 size, f32 rotation, glm::vec4 tint)
{
    if (!texture) return;
    CheckFlushCondition(texture);

    glm::vec2 center = position + size * 0.5f;
    f32 cos_r = cos(glm::radians(rotation));
    f32 sin_r = sin(glm::radians(rotation));

    glm::vec2 quadPositions[4] = {
        { position.x, position.y },
        { position.x + size.x, position.y },
        { position.x + size.x, position.y + size.y },
        { position.x, position.y + size.y }
    };

    for (int i = 0; i < 4; i++) {
        glm::vec2 p = quadPositions[i] - center;
        quadPositions[i].x = p.x * cos_r - p.y * sin_r + center.x;
        quadPositions[i].y = p.x * sin_r + p.y * cos_r + center.y;
    }

    glm::vec2 uvs[4] = { {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f} };

    for (size_t i = 0; i < 4; i++) {
        quad_buffer_ptr->position = quadPositions[i];
        quad_buffer_ptr->uv = uvs[i];
        quad_buffer_ptr->color = tint;
        quad_buffer_ptr->is_textured = 1.0f;
        quad_buffer_ptr++;
    }
    index_count += 6;
}

void UIManager::DrawSpriteFrame(Texture* texture, glm::vec2 position, glm::vec2 size, int current_frame, int total_frames, glm::vec4 tint)
{
    if (!texture) return;
    CheckFlushCondition(texture);

    glm::vec2 quadPositions[4] = {
        { position.x, position.y },
        { position.x + size.x, position.y },
        { position.x + size.x, position.y + size.y },
        { position.x, position.y + size.y }
    };

    // Calculate the slice of the texture we actually want to draw
    f32 frame_width = 1.0f / static_cast<f32>(total_frames);
    f32 u_start = current_frame * frame_width;
    f32 u_end = (current_frame + 1) * frame_width;

    glm::vec2 uvs[4] = { 
        {u_start, 1.0f}, 
        {u_end, 1.0f}, 
        {u_end, 0.0f}, 
        {u_start, 0.0f} 
    };

    // Push the four vertices to the batch buffer
    for (size_t i = 0; i < 4; i++) {
        quad_buffer_ptr->position = quadPositions[i];
        quad_buffer_ptr->uv = uvs[i];
        quad_buffer_ptr->color = tint;
        quad_buffer_ptr->is_textured = 1.0f;
        quad_buffer_ptr++;
    }
    
    index_count += 6;
}

void UIManager::DrawRect(glm::vec2 position, glm::vec2 size, glm::vec4 color, glm::vec4 bg_color, f32 fill_amount, f32 rotation)
{
    CheckFlushCondition(nullptr);

    glm::vec2 center = position + size * 0.5f;
    f32 cos_r = cos(glm::radians(rotation));
    f32 sin_r = sin(glm::radians(rotation));

    // A quick helper lambda so we don't repeat the rotation math twice
    auto PushQuad = [&](glm::vec2 pos, glm::vec2 sz, glm::vec4 col) {
        glm::vec2 quadPositions[4] = {
            { pos.x, pos.y },
            { pos.x + sz.x, pos.y },
            { pos.x + sz.x, pos.y + sz.y },
            { pos.x, pos.y + sz.y }
        };

        glm::vec2 uvs[4] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

        for (int i = 0; i < 4; i++) {
            glm::vec2 p = quadPositions[i] - center;
            glm::vec2 rotated_p;
            rotated_p.x = p.x * cos_r - p.y * sin_r + center.x;
            rotated_p.y = p.x * sin_r + p.y * cos_r + center.y;
            
            quad_buffer_ptr->position = rotated_p;
            quad_buffer_ptr->uv = uvs[i];
            quad_buffer_ptr->color = col;
            quad_buffer_ptr->is_textured = 0.0f;
            quad_buffer_ptr++;
        }

        index_count += 6;
    };

    if (bg_color.a > 0.0f) {
        PushQuad(position, size, bg_color);
    }

    if (fill_amount > 0.0f) {
        f32 clamped_fill = glm::clamp(fill_amount, 0.0f, 1.0f);
        
        f32 fg_width = size.x * clamped_fill;
        
        glm::vec2 fg_pos = position; 
        glm::vec2 fg_size = glm::vec2(fg_width, size.y);
        
        PushQuad(fg_pos, fg_size, color);
    }
}