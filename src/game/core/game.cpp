#include <game/core/game.h>

#include <game/core/game.h>
#include <GLFW/glfw3.h>
#include <engine/core/input.h>
#include <engine/audio/audio_system.h>

#include <game/ui/main_menu.h>
#include <game/scene/level_scene.h>

Game::Game()
    : delta_time(0.0f), last_frame(0.0f)
{
    // 1. Hardware and Core Systems Setup
    window.SetWindowSize(1080, 720);
    window.SetWindowTitle("SNOW DOOM");
    window.InitializeWindow();

    AudioSystem::Init();
    
    Input::Init(window.GetWindow());
    Input::BindAxis("MoveForward", KeyCode::W, KeyCode::S);
    Input::BindAxis("MoveRight", KeyCode::D, KeyCode::A);
    Input::BindAction("Interact", KeyCode::E);
    Input::BindAction("Jump", KeyCode::Space);
    Input::BindAction("Dash", KeyCode::LeftShift);
    Input::BindAction("Fire", MouseCode::Left);
    Input::BindAction("Reload", KeyCode::R);
    Input::BindAction("ToggleUI", KeyCode::Tab);
    Input::BindAction("Quit", KeyCode::Esc);

    ui_manager.Init();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    scene_manager.ChangeScene(new MainMenuScene(&window, &ui_manager));
}

Game::~Game()
{
    AudioSystem::Cleanup();
}

void Game::ProcessInput()
{
    if (Input::GetAction("Quit"))
        Exit();

    if (Input::GetActionDown("ToggleUI"))
        Input::ToggleCursor(window.GetWindow());

    scene_manager.ProcessInput();
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

        // Cap delta time to prevent physics explosions during lag spikes or window dragging
        if (delta_time > 0.1f)
            delta_time = 0.1f;

        ProcessInput();
        Update();
        Render();
    }
}

void Game::Update()
{
    scene_manager.Update(delta_time);
}

void Game::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // The scene manager handles binding shaders and drawing everything specific to that state
    scene_manager.Render(delta_time);

    ui_manager.Begin(window);
    // Any global UI like an FPS counter or debug text could go here
    ui_manager.End();

    glfwSwapBuffers(window.GetWindow());
}

void Game::Exit()
{
    glfwSetWindowShouldClose(window.GetWindow(), true);
}