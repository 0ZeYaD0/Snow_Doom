#include <game/core/game.h>
#include <engine/graphics/object_loader.h>
#include <GLFW/glfw3.h>
#include <engine/core/input.h>
#include <glm/gtc/matrix_transform.hpp>

Game::Game()
    : delta_time(0.0f), last_frame(0.0f)
{
    // init window
    window.SetWindowSize(1080, 720);
    window.SetWindowTitle("SNOW DOOM");
    window.InitializeWindow();

    Input::Init(window.GetWindow());
    // shader
    main_shader = new Shader("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");

    ui_shader = new Shader("res/shaders/ui_vertex.glsl", "res/shaders/ui_fragment.glsl");
    InitUI();
    // player
    player = new Player(glm::vec3(0.0f, 6.0f, 3.0f));

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // --- obj test
    loaded_meshes = objloader::LoadModel("res/models/cottage_obj.obj");

    for (size_t i = 0; i < loaded_meshes.size(); i++)
    {
        entities.push_back(Entity(&loaded_meshes[i]));

        // scale down and move back
        entities.back().transform.scale = glm::vec3(0.2f);
        entities.back().transform.position = glm::vec3(0.0f, -1.0f, -5.0f);
    }

    // --- ground loading
    floor_meshes = objloader::LoadModel("res/models/floor.obj");
    for (size_t i = 0; i < floor_meshes.size(); i++)
    {
        entities.push_back(Entity(&floor_meshes[i]));

        entities.back().transform.scale = glm::vec3(50.0f, 1.0f, 50.0f);
        entities.back().transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // phythix for floor
    level_colliders.push_back(AABB::fromPosSize(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(100.0f, 1.0f, 100.0f)));
}

Game::~Game()
{
    delete main_shader;
    delete ui_shader;
    delete player;
}

void Game::Run()
{
    while (!WindowShouldClose())
    {
        Input::Update();
        glfwPollEvents();

        f32 current_frame = static_cast<f32>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        if (delta_time > 0.1f)
            delta_time = 0.1f;

        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput()
{
    if (Input::GetKey(GLFW_KEY_ESCAPE))
        Exit();

    if (Input::GetKeyDown(GLFW_KEY_TAB))
        Input::ToggleCursor(window.GetWindow());
}

void Game::Update()
{
    player->UpdatePlayer(delta_time, level_colliders);

    for (auto &entity : entities)
    {
        entity.Update(delta_time);
    }
}

void Game::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    main_shader->Use();

    glm::mat4 view = player->cam.GetViewMat();
    glm::mat4 proj = player->cam.GetProjectionMat((f32)window.GetWidth() / (f32)window.GetHeight());

    main_shader->SetMat4("view", view);
    main_shader->SetMat4("projection", proj);

    main_shader->SetVec4("col", 1.0f, 0.5f, 0.2f, 1.0f);

    for (const auto &entity : entities)
    {
        entity.Draw(*main_shader);
    }

    RenderUI();

    glfwSwapBuffers(window.GetWindow());
}

void Game::Exit()
{
    glfwSetWindowShouldClose(window.GetWindow(), true);
}

void Game::InitUI()
{
    // A simple 1x1 square
    float quadVertices[] = {
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f};

    glGenVertexArrays(1, &ui_VAO);
    glGenBuffers(1, &ui_VBO);

    glBindVertexArray(ui_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glBindVertexArray(0);
}

void Game::RenderUI()
{
    // Disable 3D depth so UI is always on top
    glDisable(GL_DEPTH_TEST);

    ui_shader->Use();

    // 2D Orthographic projection
    glm::mat4 projection = glm::ortho(0.0f, (float)window.GetWidth(), (float)window.GetHeight(), 0.0f);
    ui_shader->SetMat4("projection", projection);

    glBindVertexArray(ui_VAO);

    float box_size = 30.0f;
    float padding = 10.0f;
    float start_x = 20.0f;
    float start_y = window.GetHeight() - 50.0f;

    for (int i = 0; i < player->MAX_DASH_CHARGES; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(start_x + (i * (box_size + padding)), start_y, 0.0f));
        model = glm::scale(model, glm::vec3(box_size, box_size, 1.0f));
        ui_shader->SetMat4("model", model);

        // 1. Calculate how full this specific box should be
        float fill = player->dash_recharge_timer / player->DASH_RECHARGE_TIME;
        if (i < player->dash_charges)
        {
            fill = 1.0f; // We have this charge, it is 100% full
        }
        else if (i == player->dash_charges)
        {
            // This is the exact charge that is currently recharging!
            // Divide current time by total time to get a 0.0 -> 1.0 percentage
            fill = player->dash_recharge_timer / player->DASH_RECHARGE_TIME;
        }
        else
        {
            fill = 0.0f; // We don't have this charge and it isn't recharging yet
        }

        // 2. Send the colors and the fill amount to the shader
        ui_shader->SetVec4("color", 0.0f, 1.0f, 1.0f, 1.0f);    // Bright Cyan
        ui_shader->SetVec4("bg_color", 0.2f, 0.2f, 0.2f, 1.0f); // Dark Gray
        ui_shader->SetFloat("fill_amount", fill);

        std::cout << fill << std::endl;

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);

    // Re-enable 3D depth for the next frame
    glEnable(GL_DEPTH_TEST);
}