#pragma once

#include <engine/core/defines.h>

#include <game/entities/entity.h>
#include <game/entities/hurtable.h>
#include <game/player/camera.h>
#include <engine/physics/physics.h>
#include <engine/core/input.h>

#include <vector>
#include <iostream>
using std::vector;

class Player : public Entity
{
public:
    Hurtable health;
    Camera cam;

    glm::vec3 velocity{0.0f};
    bool is_grounded = false;
    glm::vec3 player_size{1.0f, 2.0f, 1.0f};

    Player(glm::vec3 spawn_pos);

    void UpdatePlayer(f32 delta_time, const vector<AABB> &obstacles);

private:

    void HandleInput(f32 delta_time);
    void ApplyFriction(f32 delta_time);
    void Accelerate(glm::vec3 wish_dir, f32 wish_speed, f32 accel, f32 dt);
};