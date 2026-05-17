#pragma once

#include <game/core/entity.h>
#include <game/core/hurtable.h>
#include <game/player/player.h>

#include <engine/audio/audio_buffer.h>
#include <engine/audio/audio_source.h>
#include <engine/physics/physics.h>

#include <vector>
using std::vector;

enum class EnemyState
{
    IDLE,
    CHASE,
    SHOOT
};

class Enemy : public Entity
{
public:
    Enemy(glm::vec3 spawn_pos, Player *player, Mesh *mesh, Texture *tex);
    virtual ~Enemy() = default;

    void SetLevelData(vector<AABB> *colliders, vector<Entity *> *entities);

    virtual void Update(f32 delta_time) override;
    virtual void Draw(const Shader &shader) const override;
    void SetProjectileVisuals(Mesh *p_mesh, Texture *p_tex);

    AudioBuffer *sfx_death = nullptr;
    AudioBuffer *sfx_shoot = nullptr;
    AudioBuffer *sfx_hurt = nullptr;

    void PlayShootSound();
    void PlayHurtSound();
    void PlayDeathSound();

private:
    // ── identity / render
    Hurtable health;
    Player *target_player;
    Texture *enemy_tex;

    // ── physics
    glm::vec3 velocity{0.0f};
    bool is_grounded = false;
    const glm::vec3 ENEMY_SIZE{0.8f, 1.6f, 0.8f};
    const f32 GRAVITY = 25.0f;

    // ── AI config
    const f32 ATTACK_RANGE = 8.0f;
    const f32 STOP_RANGE = 2.5f;
    const f32 SHOOT_COOLDOWN = 1.4f;
    const f32 LOS_CHECK_INTERVAL = 0.12f;
    const f32 PROJECTILE_SPEED = 18.0f;
    const f32 PROJECTILE_DAMAGE = 10.0f;

    // ── AI runtime state
    EnemyState ai_state = EnemyState::IDLE;
    bool has_los = false;
    f32 los_timer = 0.0f;
    f32 shoot_timer = 0.0f;

    // ── level data
    vector<AABB> *level_colliders = nullptr;
    vector<Entity *> *entities = nullptr;

    // ── audio pool
    static const int AUDIO_POOL_SIZE = 4;
    AudioSource audio_pool[AUDIO_POOL_SIZE];
    int current_audio_index = 0;

    // ----projectile mesh
    Mesh *proj_mesh = nullptr;
    Texture *proj_tex = nullptr;

    // ── internal update steps
    void UpdatePhysics(f32 dt);
    void UpdateLOS(f32 dt);
    void UpdateAI(f32 dt);
    void Shoot();

    void PlaySoundOverlapped(AudioBuffer *buffer);

protected:
    f32 move_speed = 3.0f;
};