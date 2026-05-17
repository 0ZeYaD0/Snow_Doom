#include <game/enemies/enemy.h>
#include <game/entities/projectile.h>
#include <engine/map/mesh_generation.h>
#include <iostream>

Enemy::Enemy(glm::vec3 spawn_pos, Player *player, Mesh *mesh, Texture *tex)
    : Entity(mesh), health(100.0f), target_player(player), enemy_tex(tex)
{
    transform.position = spawn_pos;
    transform.scale = glm::vec3(1.5f, 1.0f, 1.0f);
    this->tag = CollisionTag::ENEMY;
    this->Entity::health = &this->health;

    for (int i = 0; i < AUDIO_POOL_SIZE; i++)
        audio_pool[i].SetPosition(transform.position);

    health.on_death = [this]()
    {
        std::cout << "Enemy died.\n";
        PlaySoundOverlapped(sfx_death);
        transform.scale.y = 0.1f;
        transform.position.y = 0.05f;
    };
}

void Enemy::SetLevelData(vector<AABB> *colliders, vector<Entity *> *ents)
{
    level_colliders = colliders;
    entities = ents;
}

void Enemy::Update(f32 delta_time)
{
    for (int i = 0; i < AUDIO_POOL_SIZE; i++)
        audio_pool[i].SetPosition(transform.position);

    if (health.IsDead())
        return;

    UpdatePhysics(delta_time);
    UpdateLOS(delta_time);
    UpdateAI(delta_time);
}

void Enemy::UpdatePhysics(f32 dt)
{
    if (!is_grounded)
        velocity.y -= GRAVITY * dt;

    transform.position += velocity * dt;

    if (!level_colliders)
        return;

    is_grounded = false;
    for (const auto &box : *level_colliders)
    {
        glm::vec3 old_pos = transform.position;

        if (Physics::resolveCollision(transform.position, ENEMY_SIZE, box))
        {
            if (transform.position.y > old_pos.y)
            {
                is_grounded = true;
                velocity.y = 0.0f;
            }
            else if (transform.position.y < old_pos.y && velocity.y > 0.0f)
            {
                velocity.y = 0.0f;
            }
        }
    }
}

void Enemy::UpdateLOS(f32 dt)
{
    los_timer += dt;
    if (los_timer < LOS_CHECK_INTERVAL)
        return;
    los_timer = 0.0f;

    if (!level_colliders)
    {
        has_los = false;
        return;
    }

    glm::vec3 eye_pos = transform.position + glm::vec3(0.0f, 0.8f, 0.0f);
    glm::vec3 player_eye = target_player->transform.position + glm::vec3(0.0f, 0.8f, 0.0f);

    glm::vec3 to_player = player_eye - eye_pos;
    f32 dist = glm::length(to_player);

    if (dist < 0.001f)
    {
        has_los = true;
        return;
    }

    glm::vec3 dir = to_player / dist;

    for (const auto &box : *level_colliders)
    {
        f32 hit_dist = 0.0f;
        glm::vec3 hit_norm;

        if (Physics::RayIntersectAABB(eye_pos, dir, box, hit_dist, hit_norm))
        {
            if (hit_dist > 0.05f && hit_dist < dist - 0.1f)
            {
                has_los = false;
                return;
            }
        }
    }

    has_los = true;
}

void Enemy::UpdateAI(f32 dt)
{
    glm::vec3 dir_to_player = target_player->transform.position - transform.position;
    f32 dist = glm::length(dir_to_player);

    glm::vec3 look_dir = dir_to_player;
    look_dir.y = 0.0f;
    if (glm::length(look_dir) > 0.001f)
    {
        look_dir = glm::normalize(look_dir);
        transform.rotation.y = glm::degrees(atan2(look_dir.x, look_dir.z));
    }

    // ── state transition ─────────────────────────────────────────────────────
    if (!has_los)
        ai_state = EnemyState::IDLE;
    else if (dist > ATTACK_RANGE)
        ai_state = EnemyState::CHASE;
    else
        ai_state = EnemyState::SHOOT;

    // ── behaviour ────────────────────────────────────────────────────────────
    switch (ai_state)
    {
    case EnemyState::IDLE:
        // bleed off horizontal momentum so the enemy doesn't slide after losing LOS
        velocity.x *= 0.8f;
        velocity.z *= 0.8f;
        break;

    case EnemyState::CHASE:
        if (dist > STOP_RANGE)
            transform.position += look_dir * move_speed * dt;
        break;

    case EnemyState::SHOOT:
        velocity.x *= 0.8f;
        velocity.z *= 0.8f;

        shoot_timer += dt;
        if (shoot_timer >= SHOOT_COOLDOWN)
        {
            Shoot();
            shoot_timer = 0.0f;
        }
        break;
    }
}

void Enemy::Shoot()
{
    if (!entities || !level_colliders)
        return;

    glm::vec3 spawn_pos = transform.position + glm::vec3(0.0f, 0.8f, 0.0f);
    glm::vec3 target = target_player->transform.position + glm::vec3(0.0f, 0.8f, 0.0f);
    glm::vec3 vel = glm::normalize(target - spawn_pos) * PROJECTILE_SPEED;

    Projectile *proj = new Projectile(
        spawn_pos, vel,
        PROJECTILE_DAMAGE,
        CollisionTag::ENEMY,
        level_colliders,
        entities,
        target_player,
        proj_mesh ? proj_mesh : mesh,
        proj_tex ? proj_tex : enemy_tex);

    entities->push_back(proj);
    PlaySoundOverlapped(sfx_shoot);
}

void Enemy::Draw(const Shader &shader) const
{
    shader.SetInt("useTexture", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, enemy_tex->ID);
    Entity::Draw(shader);
    shader.SetInt("useTexture", 0);
}

void Enemy::PlaySoundOverlapped(AudioBuffer *buffer)
{
    if (!buffer)
        return;
    audio_pool[current_audio_index].Play(buffer);
    current_audio_index = (current_audio_index + 1) % AUDIO_POOL_SIZE;
}

void Enemy::PlayShootSound() { PlaySoundOverlapped(sfx_shoot); }

void Enemy::PlayHurtSound()
{
    if (sfx_hurt)
        PlaySoundOverlapped(sfx_hurt);
}

void Enemy::PlayDeathSound()
{
    if (sfx_death)
        PlaySoundOverlapped(sfx_death);
}

void Enemy::SetProjectileVisuals(Mesh *p_mesh, Texture *p_tex)
{
    proj_mesh = p_mesh;
    proj_tex = p_tex;
}