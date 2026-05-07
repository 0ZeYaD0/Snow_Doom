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

void UIManager::Render(const Window &window, const Player *player)
{
    if (!player)
        return;

    glDisable(GL_DEPTH_TEST);

    ui_shader->Use();

    glm::mat4 projection = glm::ortho(0.0f, (f32)window.GetWidth(), (f32)window.GetHeight(), 0.0f);
    ui_shader->SetMat4("projection", projection);

    glBindVertexArray(VAO);

    DashCounterUI(window, player);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

void UIManager::DashCounterUI(const Window &window, const Player *player)
{
    f32 box_size = 30.0f;
    f32 padding = 10.0f;
    f32 start_x = 20.0f;
    f32 start_y = window.GetHeight() - 50.0f;

    for (i32 i = 0; i < player->GetMaxDashCharges(); i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(
                                          start_x + (i * (box_size + padding)), start_y, 0.0f));

        model = glm::scale(model, glm::vec3(box_size, box_size, 1.0f));

        ui_shader->SetMat4("model", model);

        f32 fill = 0.0f;
        if (i < player->GetDashCharges())
            fill = 1.0f;
        else if (i == player->GetDashCharges())
            fill = player->GetDashRechargeTimer() / player->GetDashRechargeTime();

        ui_shader->SetVec4("color", 0.0f, 1.0f, 1.0f, 1.0f);
        ui_shader->SetVec4("bg_color", 0.2f, 0.2f, 0.2f, 1.0f);
        ui_shader->SetFloat("fill_amount", fill);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void UIManager::DrawSprite(Texture *texture, glm::vec2 position, glm::vec2 size, glm::vec4 tint)
{
    ui_shader->Use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    ui_shader->SetMat4("model", model);
    ui_shader->SetVec4("color", tint.r, tint.g, tint.b, tint.a);
    ui_shader->SetFloat("use_texture", 1.0f);
    ui_shader->SetFloat("image", 0.0f);

    texture->Bind(0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    ui_shader->SetFloat("use_texture", 0.0f); // TODO:Float to int
    texture->Unbind();
}