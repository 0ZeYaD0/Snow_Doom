#include <game/ui/main_menu.h>

#include <engine/core/input.h>

#include <game/scene/scene_manager.h>
#include <game/scene/level_scene.h>

MainMenuScene::MainMenuScene(Window *game_window, UIManager *ui)
    : window(game_window), ui_manager(ui), title_tex(nullptr), play_btn_tex(nullptr), quit_btn_tex(nullptr)
{
    title_tex = new Texture("res/art/snow_doom_title.png");
    play_btn_tex = new Texture("res/art/play.png");
    quit_btn_tex = new Texture("res/art/quit.png");

    soundtrack = new AudioBuffer("res/audio/loading.ogg"); 
    
    audio_source.SetSpatial(false);
    audio_source.SetLooping(true);
    audio_source.Play(soundtrack);
}

void MainMenuScene::Init(SceneManager *manager)
{
    scene_manager = manager;

    f32 center_x = window->GetWidth() / 2.0f;
    f32 center_y = window->GetHeight() / 2.0f;

    // bounds are stored as x, y, width, height
    play_btn_bounds = glm::vec4(center_x - 100.0f, center_y + 100.0f, 200.0f, 60.0f);
    quit_btn_bounds = glm::vec4(center_x - 100.0f, center_y + 200.0f, 200.0f, 60.0f);

    Input::SetCursorHidden(window->GetWindow(), false);


    bg_shader = new Shader("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");

    bg_map = MapLoader::Load("res/maps/main_menu.map");
    camera_angle = 0.0f;
}

void MainMenuScene::ProcessInput()
{
    if (Input::GetActionDown("Fire"))
    {
        auto mouse_pos = Input::GetMousePosition();
        f32 mx = mouse_pos.x;
        f32 my = mouse_pos.y;

        if (CheckButtonClick(play_btn_bounds, mx, my))
        {
            Window* win = window;
            UIManager* ui = ui_manager;
            SceneManager* sm = scene_manager;

            // Set up Level 1
            auto lvl1 = new LevelScene("res/maps/level_1.map", win, ui);
            lvl1->SetSoundtrack("res/audio/at_dooms_gate.ogg");

            lvl1->SetCompletionLogic(
                [lvl1]() -> bool {
                    return lvl1->GetPlayer() != nullptr && lvl1->GetPlayer()->HasKey("end_key");
                },
                [win, ui, sm]() -> void {
                    auto lvl2 = new LevelScene("res/maps/level_2.map", win, ui);
                    lvl2->SetSoundtrack("res/audio/sign_of_evil.ogg");
                    
                    lvl2->SetCompletionLogic(
                        [lvl2]() -> bool {
                            return lvl2->GetPlayer() != nullptr && lvl2->GetPlayer()->HasKey("end_key");
                        },
                        [win, ui, sm]() -> void {
                            auto menu = new MainMenuScene(win, ui);
                            sm->ChangeScene(menu);
                        }
                    );

                    sm->ChangeScene(lvl2);
                }
            );

            scene_manager->ChangeScene(lvl1);
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
    camera_angle += 15.0f * dt;
    return;
}

void MainMenuScene::Render(f32 dt)
{
    // Clear screen
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- 1. DRAW THE 3D ROTATING ROOM ---
    bg_shader->Use();

    // Orbit math: Radius of 15 units from the center (0,0,0)
    f32 radius = 15.0f;
    f32 cam_x = sin(glm::radians(camera_angle)) * radius;
    f32 cam_z = cos(glm::radians(camera_angle)) * radius;

    // Setup the view matrix (Camera position, Target position, Up direction)
    // We lift the camera 5.0f units in the air, looking slightly down at 2.0f
    glm::mat4 view = glm::lookAt(glm::vec3(cam_x, 5.0f, cam_z), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::perspective(glm::radians(70.0f), (f32)window->GetWidth() / (f32)window->GetHeight(), 0.1f, 100.0f);

    bg_shader->SetMat4("view", view);
    bg_shader->SetMat4("projection", proj);
    bg_shader->SetMat4("model", glm::mat4(1.0f));

    // Optional: Dim the background slightly darker than the main game so the UI pops out more
    bg_shader->SetVec4("col", 0.4f, 0.4f, 0.4f, 1.0f);

    bg_shader->SetInt("u_Texture", 0);
    bg_shader->SetInt("useTexture", 1);

    // Draw the static level geometry
    for (const auto &map_ent : bg_map.entities)
    {
        if (map_ent.texture)
            map_ent.texture->Bind(0);
        map_ent.mesh.Draw(*bg_shader);
    }
    bg_shader->SetInt("useTexture", 0);

    ui_manager->Begin(*window);

    auto mouse_pos = Input::GetMousePosition();
    f32 mx = mouse_pos.x;
    f32 my = mouse_pos.y;

    glm::vec4 play_color = CheckButtonClick(play_btn_bounds, mx, my) ? glm::vec4(0.7f, 0.7f, 0.7f, 1.0f) : glm::vec4(1.0f);
    glm::vec4 quit_color = CheckButtonClick(quit_btn_bounds, mx, my) ? glm::vec4(0.7f, 0.7f, 0.7f, 1.0f) : glm::vec4(1.0f);

    glm::vec2 title_size(600.0f, 300.0f);
    glm::vec2 title_pos((window->GetWidth() / 2.0f) - (title_size.x / 2.0f), 80.0f);
    if (title_tex)
    {
        ui_manager->DrawSprite(title_tex, title_pos, title_size, 0.0f, glm::vec4(1.0f));
    }

    if (play_btn_tex)
    {
        ui_manager->DrawSprite(
            play_btn_tex,
            {play_btn_bounds.x, play_btn_bounds.y},
            {play_btn_bounds.z, play_btn_bounds.w},
            0.0f, play_color);
    }

    if (quit_btn_tex)
    {
        ui_manager->DrawSprite(
            quit_btn_tex,
            {quit_btn_bounds.x, quit_btn_bounds.y},
            {quit_btn_bounds.z, quit_btn_bounds.w},
            0.0f, quit_color);
    }

    ui_manager->End();
}

void MainMenuScene::Cleanup()
{
    if (title_tex)
        delete title_tex;
    if (play_btn_tex)
        delete play_btn_tex;
    if (quit_btn_tex)
        delete quit_btn_tex;
    if (bg_shader)
        delete bg_shader;
}

bool MainMenuScene::CheckButtonClick(glm::vec4 bounds, f32 mouse_x, f32 mouse_y)
{
    return mouse_x >= bounds.x && mouse_x <= bounds.x + bounds.z &&
           mouse_y >= bounds.y && mouse_y <= bounds.y + bounds.w;
}