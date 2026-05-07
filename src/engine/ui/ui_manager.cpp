#include <engine/ui/ui_manager.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

UIManager::UIManager()
    : ui_shader(nullptr), VAO(0), VBO(0)
{
}

UIManager::~UIManager()
{
    if (ui_shader)
        delete ui_shader;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void UIManager::Init()
{
    ui_shader = new Shader("res/shaders/ui_vertex.glsl", "res/shaders/ui_fragment.glsl");

    f32 quad_vertices[] = {
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void *)0);
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

    glBindVertexArray(VAO);
}

void UIManager::End()
{
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void UIManager::DrawSprite(Texture* texture, glm::vec2 position, glm::vec2 size, f32 rotation, glm::vec4 tint)
{
    if (!texture) return;

    ui_shader->Use();
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));

    model = glm::translate(model, glm::vec3(size.x * 0.5f, size.y * 0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-size.x * 0.5f, -size.y * 0.5f, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f)); 

    ui_shader->SetMat4("model", model);
    ui_shader->SetVec4("color", tint.r, tint.g, tint.b, tint.a);
    ui_shader->SetInt("use_texture", 1);

    // Reset scale and offset for full sprites
    ui_shader->SetVec2("uv_scale", 1.0f, 1.0f);
    ui_shader->SetVec2("uv_offset", 0.0f, 0.0f);

    texture->Bind(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    ui_shader->SetInt("use_texture", 0);
    texture->Unbind();
}

void UIManager::DrawSpriteFrame(Texture* texture, glm::vec2 position, glm::vec2 size, int current_frame, int total_frames, glm::vec4 tint)
{
    if (!texture) return;

    ui_shader->Use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    ui_shader->SetMat4("model", model);
    ui_shader->SetVec4("color", tint.r, tint.g, tint.b, tint.a);
    ui_shader->SetInt("use_texture", 1);

    f32 frame_width = 1.0f / static_cast<f32>(total_frames);
    ui_shader->SetVec2("uv_scale", frame_width, 1.0f);
    ui_shader->SetVec2("uv_offset", current_frame * frame_width, 0.0f);

    texture->Bind(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    ui_shader->SetVec2("uv_scale", 1.0f, 1.0f);
    ui_shader->SetVec2("uv_offset", 0.0f, 0.0f);
    ui_shader->SetInt("use_texture", 0);
    texture->Unbind();
}

void UIManager::DrawRect(glm::vec2 position, glm::vec2 size, glm::vec4 color, glm::vec4 bg_color, f32 fill_amount, f32 rotation)
{
    ui_shader->Use();
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));  
    
    model = glm::translate(model, glm::vec3(size.x * 0.5f, size.y * 0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-size.x * 0.5f, -size.y * 0.5f, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f)); 

    ui_shader->SetMat4("model", model);
    ui_shader->SetVec4("color", color.r, color.g, color.b, color.a);
    ui_shader->SetVec4("bg_color", bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    ui_shader->SetFloat("fill_amount", fill_amount);
    ui_shader->SetInt("use_texture", 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}