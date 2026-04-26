#include <game.h>
#include <graphics/object_loader.h>
#include <GLFW/glfw3.h>

Game::Game()
    : delta_time(0.0f), last_frame(0.0f)
{
    // init window
    window.SetWindowSize(800, 600);
    window.SetWindowTitle("SNOW DOOM");
    window.InitializeWindow();

    // shader
    main_shader = new Shader("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");

    // cam
    cam = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // --- obj test
    loaded_meshes = objloader::LoadModel("res/models/snowman.obj");
    if(!loaded_meshes.empty())
        entities.push_back(Entity(&loaded_meshes[0]));
}

Game::~Game()
{
    delete main_shader;
}

void Game::Run()
{
    while(!WindowShouldClose()) {

        // delta time
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
    // sike
}

void Game::Update()
{
    for(auto &entity : entities)
    {
        entity.Update(delta_time);
    }
}

void Game::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    main_shader->Use();

    glm::mat4 view = cam.GetViewMat();
    glm::mat4 proj = cam.GetProjectionMat((f32)window.GetWidth() / (f32)window.GetHeight());

    main_shader->SetMat4("view", view);
    main_shader->SetMat4("projection", proj);

    main_shader->SetVec4("col", 1.0f, 0.5f, 0.2f, 1.0f);

    for(const auto &entity : entities) {
        entity.Draw(*main_shader);
    }

    glfwSwapBuffers(window.GetWindow());
    glfwPollEvents();
}

void Game::Exit()
{
    glfwSetWindowShouldClose(window.GetWindow(), true);
}