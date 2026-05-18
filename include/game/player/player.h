#pragma once

#include <engine/core/defines.h>

#include <game/core/entity.h>
#include <game/core/hurtable.h>
#include <game/player/camera.h>
#include <engine/physics/physics.h>
#include <engine/core/input.h>
#include <game/player/weapon/weapon.h>

#include <engine/audio/audio_buffer.h>
#include <engine/audio/audio_source.h>

#include <vector>
#include <iostream>
using std::vector;

class Player : public Entity
{
private:
    // CONFIG
    const f32 MAX_HEALTH = 100.0f;
    const f32 GRAVITY_MULT = 25.0f;

    // Movement Physics
    const f32 MAX_GROUND_SPEED = 12.0f;
    const f32 GROUND_ACCEL = 10.0f;
    const f32 MAX_AIR_SPEED = 13.0f;
    const f32 AIR_ACCEL = 2.0f;
    const f32 GROUND_FRICTION = 13.0f;
    const f32 STOP_SPEED_THRESHOLD = 0.1f;
    const f32 JUMP_FORCE = 10.0f;

    // Dash
    const f32 DASH_SPEED = 25.0f;
    const f32 DASH_JUMP_FORCE = 2.0f;
    const int MAX_DASH_CHARGES = 3;
    const f32 DASH_RECHARGE_TIME = 1.0f;

    // Camera & Polish
    const f32 CAM_Y_POS = 0.8f;
    const f32 CAM_MAX_TILT = 1.0f;
    const f32 CAM_TILT_SPEED = 10.0f;

    const f32 BOB_MIN_SPEED = 0.1f;
    const f32 BOB_MOVE_AMP = 0.05f;
    const f32 BOB_MOVE_FREQ = 12.0f;
    const f32 BOB_IDLE_AMP = 0.005f;
    const f32 BOB_IDLE_FREQ = 2.0f;
    const f32 BOB_SMOOTHING = 10.0f;
    const f32 FALL_VELOCITY_THRESHOLD = 1.0f;

    // Weapons (TEMP)
    const f32 BASE_RECOIL_PITCH = 2.0f;

    // RUNTIME STATE
    int dash_charges = MAX_DASH_CHARGES;
    f32 dash_recharge_timer = 0.0f;

    f32 bob_phase = 0.0f;
    f32 current_bob_amp = 0.0f;
    bool head_bob = true;

    f32 speed_lines_opacity = 0.0f;

public:
    glm::vec3 velocity{0.0f};
    bool is_grounded = false;
    glm::vec3 player_size{1.0f, 2.0f, 1.0f};
    bool is_dead = false;
    bool IsDead() const { return is_dead; }

    // COMPONENTS
    Hurtable health;
    Camera cam;
    Weapon *current_weapon = nullptr;

    // --- AUDIO ---
    AudioBuffer *sfx_shoot = nullptr;
    AudioBuffer *sfx_footstep = nullptr;
    void PlayShootSound();

    // METHODS
    Player(glm::vec3 spawn_pos);
    ~Player();

    // INVENTORY
    vector<string> inventory_keys;

    void GiveKey(const string &key_id);
    bool HasKey(const string &key_id);
    void EquipWeapon(const string &weapon_id);

    void UpdatePlayer(f32 delta_time, const vector<AABB> &obstacles, vector<Entity *> &entities);

private:
    // --- ARCH ---
    void UpdateTimers(f32 delta_time);
    void UpdateMouseLook();
    void HandleInput(f32 delta_time, vector<Entity *> &entities);
    void UpdatePhysics(f32 delta_time, const vector<AABB> &obstacles);
    void UpdateCameraEffects(f32 delta_time);

    // --- PHYSICS ---
    void ApplyFriction(f32 delta_time);
    void Accelerate(glm::vec3 wish_dir, f32 wish_speed, f32 accel, f32 dt);

    // --- CAM ---
    void UpdateCameraTilt(f32 delta_time);
    void HeadBob(f32 delta_time);
    void UpdateSpeedLines(f32 delta_time);

    // --- AUDIO POOL ---
    static const int AUDIO_POOL_SIZE = 4;
    AudioSource audio_pool[AUDIO_POOL_SIZE];
    int current_audio_index = 0;

    void PlaySoundOverlapped(AudioBuffer *buffer);

public:
    // getters/setters
    i32 GetDashCharges() const { return dash_charges; }
    i32 GetMaxDashCharges() const { return MAX_DASH_CHARGES; }
    f32 GetDashRechargeTimer() const { return dash_recharge_timer; }
    f32 GetDashRechargeTime() const { return DASH_RECHARGE_TIME; }
    f32 GetSpeedLinesOpacity() const { return speed_lines_opacity; }

    f32 GetHealth() const { return health.GetCurrHP(); }
    f32 GetMaxHealth() const { return health.GetMaxHP(); }
    Weapon *GetActiveWeapon() const { return current_weapon; }
};