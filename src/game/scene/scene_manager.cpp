#include <game/scene/scene_manager.h>

SceneManager::~SceneManager()
{
    if(current_scene)
    {
        current_scene->Cleanup();
        delete current_scene;
    }
}

void SceneManager::ChangeScene(Scene *new_scene)
{
    next_scene = new_scene;
}

void SceneManager::ProcessInput()
{
    if(current_scene) current_scene->ProcessInput();
}

void SceneManager::Update(f32 dt)
{
    if(next_scene != nullptr)
    {
        if(current_scene != nullptr)
        {
            current_scene->Cleanup();
            delete current_scene;
        }

        current_scene = next_scene;
        next_scene = nullptr;

        current_scene->Init(this);
    }

    if(current_scene) current_scene->Update(dt);
}

void SceneManager::Render(f32 dt)
{
    if(current_scene) current_scene->Render(dt);
}