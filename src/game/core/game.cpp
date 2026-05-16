#include <game/core/game.h>
#include <engine/graphics/object_loader.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <engine/core/input.h>
#include <glm/gtc/matrix_transform.hpp>

#include <game/enemies/enemy.h>
#include <engine/map/mesh_generation.h>
#include <game/entities/door.h>

#include <engine/audio/audio_system.h>
#include <engine/audio/audio_source.h>

#include <game/entities/pickup.h>
#include <game/entities/door.h>

Game::Game()
    : delta_time(0.0f), last_frame(0.0f)
{
    // init window
    window.SetWindowSize(1080, 720);
    window.SetWindowTitle("SNOW DOOM");
    window.InitializeWindow();

    // AUDIO TEST
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

    // shaders
    main_shader = new Shader("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");

    // ui system
    ui_manager.Init();
    player_hud.Init();

    // player
    player = new Player(glm::vec3(0.0f, 4.0f, 0.0f));
    AudioBuffer *snd_player_shoot = new AudioBuffer("res/audio/shoot.ogg");
    player->sfx_shoot = snd_player_shoot;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // --- obj test
    current_map = MapLoader::Load("res/maps/level_2.map");

    for (size_t i = 0; i < current_map.colliders.size(); i++)
    {
        level_colliders.push_back(current_map.colliders[i]);
    }

    Mesh *sprite_quad = new Mesh(BuildSpriteQuad());
    AudioBuffer *snd_death = new AudioBuffer("res/audio/death/4.ogg");

    for (const auto &enemy_data : current_map.enemies)
    {
        Enemy *new_enemy = new Enemy(
            enemy_data.pos,
            player,
            sprite_quad,
            enemy_data.texture.get());

        new_enemy->sfx_death = snd_death;

        entities.push_back(new_enemy);
    }

    // --- INTERACTION SYSTEM TEST SETUP ---
    Texture *key_tex = new Texture("res/art/key.png");
    Pickup *test_key = new Pickup(
        glm::vec3(5.0f, 1.0f, 5.0f),
        player,
        sprite_quad,
        key_tex,
        PickupType::KEY,
        "red_key");
    entities.push_back(test_key);

    Texture *banana_tex = new Texture("res/art/fruit_banana.png");
    Pickup *test_weapon = new Pickup(
        glm::vec3(-5.0f, 1.0f, 5.0f),
        player,
        sprite_quad,
        banana_tex,
        PickupType::WEAPON,
        "banana_gun");
    entities.push_back(test_weapon);

    Texture *door_tex = new Texture("res/texture/Doors/Doors01.png");
    Door *test_door = new Door(
        glm::vec3(0.0f, 1.5f, 7.0f),
        glm::vec3(2.0f, 3.0f, 0.5f),
        player,
        sprite_quad,
        door_tex,
        "red_key");
    entities.push_back(test_door);
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
    // TODO: refactor
    AudioSystem::Cleanup();

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
    for (auto &entity : entities)
    {
        entity->Update(delta_time);
    }

    entities.erase(std::remove_if(entities.begin(), entities.end(),
                                  [](Entity *e)
                                  {
                                      if (e->pending_destroy)
                                      {
                                          delete e;
                                          return true;
                                      }
                                      return false;
                                  }),
                   entities.end());

    vector<AABB> current_frame_colliders = level_colliders;

    for (auto &entity : entities)
    {
        Door *door = dynamic_cast<Door *>(entity);
        if (door != nullptr)
        {
            if (!door->IsOpen())
            {
                current_frame_colliders.push_back(door->GetCollider());
            }
        }
    }

    player->UpdatePlayer(delta_time, current_frame_colliders, entities);
}

void Game::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    main_shader->Use();

    glm::mat4 view = player->cam.GetViewMat();
    glm::mat4 proj = player->cam.GetProjectionMat((f32)window.GetWidth() / (f32)window.GetHeight());

    main_shader->SetMat4("view", view);
    main_shader->SetMat4("projection", proj);
    main_shader->SetMat4("model", glm::mat4(1.0f));

    main_shader->SetVec4("col", 1.0f, 0.5f, 0.2f, 1.0f);

    main_shader->SetInt("u_Texture", 0);
    main_shader->SetInt("useTexture", 1);

    for (const auto &map_ent : current_map.entities)
    {
        if (map_ent.texture)
            map_ent.texture->Bind(0);

        map_ent.mesh.Draw(*main_shader);
    }

    main_shader->SetInt("useTexture", 0);

    for (const auto &entity : entities)
    {
        entity->Draw(*main_shader);
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