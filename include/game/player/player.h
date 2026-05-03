#pragma once

#include <engine/core/defines.h>

#include <game/entities/entity.h>
#include <game/entities/hurtable.h>
#include <game/player/camera.h>
#include <engine/physics/physics.h>
#include <engine/core/input.h>
#include <game/player/weapon/weapon.h>

#include <vector>
#include <iostream>
using std::vector;

class Player : public Entity
{
public:
    Hurtable health;
    Camera cam;
    Weapon *current_weapon = nullptr;

    // Dashs
    const f32 DASH_SPEED = 35.0f;
    const int MAX_DASH_CHARGES = 3;
    int dash_charges = 3;
    const f32 DASH_RECHARGE_TIME = 2.0f;
    f32 dash_recharge_timer = 0.0f;

    glm::vec3 velocity{0.0f};
    bool is_grounded = false;
    glm::vec3 player_size{1.0f, 2.0f, 1.0f};

    Player(glm::vec3 spawn_pos);
    ~Player();

    void UpdatePlayer(f32 delta_time, const vector<AABB> &obstacles, vector<Entity> &entities);

private:
    void HandleInput(f32 delta_time, vector<Entity> &entities);
    void ApplyFriction(f32 delta_time);
    void Accelerate(glm::vec3 wish_dir, f32 wish_speed, f32 accel, f32 dt);
};