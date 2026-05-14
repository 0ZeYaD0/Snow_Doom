#include <game/enemies/enemy.h>

#include <engine/map/mesh_generation.h>

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
        std::cout << "I DIED. WHY" << std::endl;
        PlaySoundOverlapped(sfx_death);
        transform.scale.y = 0.1f;
        transform.position.y = 0.05f;
    };
}

void Enemy::Update(f32 delta_time)
{
    for (int i = 0; i < AUDIO_POOL_SIZE; i++)
        audio_pool[i].SetPosition(transform.position);

    glm::vec3 dir_to_player = target_player->transform.position - transform.position;

    glm::vec3 look_dir = dir_to_player;
    look_dir.y = 0.0f;
    look_dir = glm::normalize(look_dir);
    f32 yaw = atan2(look_dir.x, look_dir.z);
    transform.rotation.y = glm::degrees(yaw);

    if (health.IsDead())
        return;

    f32 dist = glm::length(dir_to_player);
    if (dist > 1.5f)
    {
        transform.position += look_dir * move_speed * delta_time;
    }
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

void Enemy::PlayShootSound()
{
    PlaySoundOverlapped(sfx_shoot);
}

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