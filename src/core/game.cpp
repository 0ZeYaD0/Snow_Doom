#include <core/game.h>
#include <graphics/object_loader.h>
#include <GLFW/glfw3.h>
#include <core/inputs.h>

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

    // cam
    cam = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

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
}

Game::~Game()
{
    delete main_shader;
}

void Game::Run()
{
    while (!WindowShouldClose())
    {
        // 1. Reset input single-frame states (deltas, key down/up)
        Input::Update();

        // 2. Poll OS events. This fires your Input callbacks to get the new mouse position!
        glfwPollEvents();

        // 3. Calculate delta time
        f32 current_frame = static_cast<f32>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // 4. Process the freshly gathered input and update game
        ProcessInput();
        Update();

        // 5. Draw the frame
        Render();
    }
}

void Game::ProcessInput()
{
    // Close the game if Escape is pressed
    if (Input::GetKey(GLFW_KEY_ESCAPE))
    {
        Exit();
    }

    // --- Camera Look (Mouse) ---
    glm::vec2 mouseDelta = Input::GetMouseDelta();
    if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
    {
        // Adjust this method name to whatever handles pitch/yaw in your camera.h
        cam.ProcessMouseMov(mouseDelta.x, mouseDelta.y);
    }

    // --- Camera Movement (Keyboard) ---
    // Adjust FORWARD, BACKWARD, etc., to match the enums/methods in your camera.h
    if (Input::GetKey(GLFW_KEY_W))
        cam.ProcessKeyboard(FORWARD, delta_time);

    if (Input::GetKey(GLFW_KEY_S))
        cam.ProcessKeyboard(BACKWARD, delta_time);

    if (Input::GetKey(GLFW_KEY_A))
        cam.ProcessKeyboard(LEFT, delta_time);

    if (Input::GetKey(GLFW_KEY_D))
        cam.ProcessKeyboard(RIGHT, delta_time);

    // --- Utility ---
    // Toggle the cursor lock on/off with TAB (useful for debugging/closing)
    if (Input::GetKeyDown(GLFW_KEY_TAB))
    {
        Input::ToggleCursor(window.GetWindow());
    }
}

void Game::Update()
{
    for (auto &entity : entities)
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