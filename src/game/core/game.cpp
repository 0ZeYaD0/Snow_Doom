#include <game/core/game.h>
#include <engine/graphics/object_loader.h>
#include <GLFW/glfw3.h>
#include <engine/core/input.h>

Game::Game()
    : delta_time(0.0f), last_frame(0.0f)
{
    // init window
    window.SetWindowSize(800, 600);
    window.SetWindowTitle("SNOW DOOM");
    window.InitializeWindow();

    Input::Init(window.GetWindow());
    // shader
    main_shader = new Shader("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");

    // player
    player = new Player(glm::vec3(0.0f, 2.0f, 3.0f));

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
        glm::vec3(0.0f, -0.5f, 0.0f), 
        glm::vec3(100.0f, 1.0f, 100.0f)
    ));
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

    glfwSwapBuffers(window.GetWindow());
}

void Game::Exit()
{
    glfwSetWindowShouldClose(window.GetWindow(), true);
}