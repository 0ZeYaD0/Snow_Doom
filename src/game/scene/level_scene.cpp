#include <game/scene/level_scene.h>

#include <engine/core/input.h>
#include <engine/audio/audio_buffer.h>
#include <engine/map/mesh_generation.h>

#include <game/enemies/enemy.h>
#include <game/entities/pickup.h>
#include <game/entities/door.h>
#include <game/entities/projectile.h>

#include <algorithm>

LevelScene::LevelScene(const string &map_file, Window *game_window, UIManager *ui)
    : map_filepath(map_file), window(game_window), ui_manager(ui), player(nullptr), main_shader(nullptr)
{
}

void LevelScene::Init(SceneManager *manager)
{
    Input::SetCursorHidden(window->GetWindow(), true);

    scene_manager = manager;
    you_died_file = "res/audio/you_died.ogg";

    // shaders
    main_shader = new Shader("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");

    // hud
    player_hud.Init();

    // player
    player = new Player(glm::vec3(0.0f, 4.0f, 0.0f));

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // map
    current_map = MapLoader::Load(map_filepath);
    for (size_t i = 0; i < current_map.colliders.size(); i++)
        level_colliders.push_back(current_map.colliders[i]);

    current_frame_colliders = level_colliders;

    Mesh *sprite_quad = new Mesh(BuildSpriteQuad());
    AudioBuffer *snd_death = new AudioBuffer("res/audio/death.ogg");
    AudioBuffer *snd_enemy_shoot = new AudioBuffer("res/audio/snowball_sound.ogg");
    Texture *enemy_bullet_tex = new Texture("res/art/snowball.png");

    for (const auto &enemy_data : current_map.enemies)
    {
        Enemy *new_enemy = new Enemy(
            enemy_data.pos,
            player,
            sprite_quad,
            enemy_data.texture.get());

        new_enemy->sfx_death = snd_death;
        new_enemy->sfx_shoot = snd_enemy_shoot;

        new_enemy->SetLevelData(&current_frame_colliders, &entities);
        new_enemy->SetProjectileVisuals(sprite_quad, enemy_bullet_tex);
        entities.push_back(new_enemy);
    }

    // ── pickups
    for (const auto &pickup_data : current_map.pickups)
    {
        Pickup *new_pickup = new Pickup(
            pickup_data.pos,
            player,
            sprite_quad,
            pickup_data.texture.get(),
            pickup_data.type,
            pickup_data.item_id);
        entities.push_back(new_pickup);
    }

    // ── doors
    for (const auto &door_data : current_map.doors)
    {
        Mesh *door_mesh = new Mesh(
            MapLoader::BuildCubeMesh(glm::vec3(0.0f), door_data.size, door_data.should_tile));

        Door *new_door = new Door(
            door_data.pos,
            door_data.size,
            player,
            door_mesh,
            door_data.texture.get(),
            door_data.req_key);
        entities.push_back(new_door);
    }

    bg_music = new AudioBuffer(soundtrack_file);
    you_died_music = new AudioBuffer(you_died_file);

    bg_audio_source.SetSpatial(false);
    bg_audio_source.SetLooping(true);
    bg_audio_source.SetVolume(0.3f);
    bg_audio_source.Play(bg_music);
}

void LevelScene::ProcessInput()
{
    return;
}

void LevelScene::Update(f32 dt)
{
    if (player && player->IsDead())
    {
        if (!played_death_sound)
        {
            bg_audio_source.Stop();

            bg_audio_source.SetSpatial(false);
            bg_audio_source.SetLooping(false);
            bg_audio_source.SetVolume(1.0f);
            bg_audio_source.Play(you_died_music);

            played_death_sound = true;
        }

        if (Input::GetActionDown("Jump") || Input::GetActionDown("Fire"))
            Reset();

        return;
    }

    // This loop now automatically updates projectiles spawned by the player or enemies
    for (auto &entity : entities)
    {
        entity->Update(dt);
    }

    // Automatically cleans up projectiles when they flag themselves as pending_destroy
    entities.erase(std::remove_if(
                       entities.begin(), entities.end(),
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

    std::vector<AABB> current_frame_colliders = level_colliders;

    for (auto &entity : entities)
    {
        Door *door = dynamic_cast<Door *>(entity);
        if (door != nullptr && !door->IsOpen())
        {
            current_frame_colliders.push_back(door->GetCollider());
        }
    }

    player->UpdatePlayer(dt, current_frame_colliders, entities);
    if (!is_completed && check_completion && check_completion())
    {
        is_completed = true;
        if (on_level_complete)
        {
            on_level_complete();
        }
    }
}

void LevelScene::Render(f32 dt)
{
    main_shader->Use();

    glm::mat4 view = player->cam.GetViewMat();
    glm::mat4 proj = player->cam.GetProjectionMat((f32)window->GetWidth() / (f32)window->GetHeight());

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

    // This loop automatically renders the projectiles
    for (const auto &entity : entities)
    {
        entity->Draw(*main_shader);
    }

    ui_manager->Begin(*window);
    player_hud.Render(*ui_manager, *window, player, dt);
    ui_manager->End();
}

void LevelScene::Cleanup()
{
    delete main_shader;
    delete player;
    for (auto e : entities)
        delete e;
    entities.clear();

    level_colliders.clear();
    current_frame_colliders.clear();

    if (bg_music)
    {
        delete bg_music;
        bg_music = nullptr;
    }

    if (you_died_music)
    {
        delete you_died_music;
        you_died_music = nullptr;
    }
}

void LevelScene::Reset()
{
    bg_audio_source.Stop();
    Cleanup();

    is_completed = false;
    played_death_sound = false;

    Init(scene_manager);
}

void LevelScene::SetCompletionLogic(std::function<bool()> condition, std::function<void()> callback)
{
    check_completion = condition;
    on_level_complete = callback;
}