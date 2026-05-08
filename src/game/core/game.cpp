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

    Input::BindAxis("MoveForward", KeyCode::W, KeyCode::S);
    Input::BindAxis("MoveRight", KeyCode::D, KeyCode::A);

    Input::BindAction("Jump", KeyCode::Space);
    Input::BindAction("Dash", KeyCode::LeftShift);
    Input::BindAction("Fire", MouseCode::Left);
    Input::BindAction("Reload", KeyCode::R);

    Input::BindAction("ToggleUI", KeyCode::Tab);
    Input::BindAction("Quit", KeyCode::Esc);

    // shaders
    main_shader = new Shader("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");

    // ui system
    ui_manager.Init();
    player_hud.Init();

    // player
    player = new Player(glm::vec3(0.0f, 4.0f, 0.0f));

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // --- obj test
    current_map = MapLoader::Load("res/maps/test.map");

    for (size_t i = 0; i < current_map.meshes.size(); i++)
    {
        entities.push_back(Entity(&current_map.meshes[i]));
    }

    for (size_t i = 0; i < current_map.colliders.size(); i++)
    {
        level_colliders.push_back(current_map.colliders[i]);
    }
}

void Game::ProcessInput()
{
    if (Input::GetAction("Quit"))
        Exit();

    if (Input::GetActionDown("ToggleUI"))
        Input::ToggleCursor(window.GetWindow());
}

Game::~Game()
{
    delete main_shader;
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

void Game::Update()
{
    player->UpdatePlayer(delta_time, level_colliders, entities);

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

    ui_manager.Begin(window);
    player_hud.Render(ui_manager, window, player, delta_time);
    ui_manager.End();

    glfwSwapBuffers(window.GetWindow());
}

void Game::Exit()
{
    glfwSetWindowShouldClose(window.GetWindow(), true);
}