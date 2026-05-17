#include <game/ui/main_menu.h>

#include <engine/core/input.h>

#include <game/scene/scene_manager.h>
#include <game/scene/level_scene.h>

MainMenuScene::MainMenuScene(Window* game_window, UIManager* ui)
    : window(game_window), ui_manager(ui), title_tex(nullptr), play_btn_tex(nullptr), quit_btn_tex(nullptr)
{
}

void MainMenuScene::Init(SceneManager *manager)
{
    scene_manager = manager;

    f32 center_x = window->GetWidth() / 2.0f;
    f32 center_y = window->GetHeight() / 2.0f;

    // bounds are stored as x, y, width, height
    play_btn_bounds = glm::vec4(center_x - 100.0f, center_y - 20.0f, 200.0f, 60.0f);
    quit_btn_bounds = glm::vec4(center_x - 100.0f, center_y + 60.0f, 200.0f, 60.0f);

    Input::ToggleCursor(window->GetWindow());
}

void MainMenuScene::ProcessInput()
{
    if(Input::GetActionDown("Fire"))
    {
        auto mouse_pos = Input::GetMousePosition();
        f32 mx = mouse_pos.x;
        f32 my = mouse_pos.y;

        if (CheckButtonClick(play_btn_bounds, mx, my))
        {
            // play game
            scene_manager->ChangeScene(new LevelScene("res/maps/test1.map", window, ui_manager));
        }
        else if (CheckButtonClick(quit_btn_bounds, mx, my))
        {
            // hit quit
            window->SetWindowShouldClose(true);
        }
    }
}

void MainMenuScene::Update(f32 dt)
{
    return;
}

void MainMenuScene::Render(f32 dt)
{
    // Clear screen to a dark aesthetic background
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ui_manager->Begin(*window);
    
    auto mouse_pos = Input::GetMousePosition();
    f32 mx = mouse_pos.x;
    f32 my = mouse_pos.y;

    glm::vec4 play_color = CheckButtonClick(play_btn_bounds, mx, my) ? glm::vec4(0.7f, 0.7f, 0.7f, 1.0f) : glm::vec4(1.0f);
    glm::vec4 quit_color = CheckButtonClick(quit_btn_bounds, mx, my) ? glm::vec4(0.7f, 0.7f, 0.7f, 1.0f) : glm::vec4(1.0f);

    glm::vec4 bg_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

    ui_manager->DrawRect({play_btn_bounds.x, play_btn_bounds.y}, {play_btn_bounds.z, play_btn_bounds.w}, play_color, bg_color, 1.0f);
    ui_manager->DrawRect({quit_btn_bounds.x, quit_btn_bounds.y}, {quit_btn_bounds.z, quit_btn_bounds.w}, quit_color, bg_color, 1.0f);
    
    ui_manager->End();
}

void MainMenuScene::Cleanup()
{
    if (title_tex) delete title_tex;
    if (play_btn_tex) delete play_btn_tex;
    if (quit_btn_tex) delete quit_btn_tex;
}

bool MainMenuScene::CheckButtonClick(glm::vec4 bounds, f32 mouse_x, f32 mouse_y)
{
    return mouse_x >= bounds.x && mouse_x <= bounds.x + bounds.z &&
           mouse_y >= bounds.y && mouse_y <= bounds.y + bounds.w;
}