#include <game/enemies/enemy.h>

#include <engine/map/mesh_generation.h>

Enemy::Enemy(glm::vec3 spawn_pos, Player *player, Mesh *mesh, Texture *tex)
    : Entity(mesh), health(100.0f), target_player(player), enemy_tex(tex)
{
    transform.position = spawn_pos;

    // transform.scale = glm::vec3(1.5f, 2.5f, 1.0f);

    this->tag = CollisionTag::ENEMY;

    this->Entity::health = &this->health;

    health.on_death = [this]() {
        std::cout << "I DIED. WHY" << std::endl;
        // TODO: dieeeee
    };
}

void Enemy::Update(f32 delta_time)
{
    if(health.IsDead()) return;

    glm::vec3 dir_to_player = target_player->transform.position - transform.position;
    f32 dist = glm::length(dir_to_player);

    if(dist > 1.5f)
    {
        glm::vec3 move_dir = glm::normalize(dir_to_player);
        move_dir.y = 0.0f;
        transform.position += move_dir * move_speed * delta_time;
    }

    // billboard
    glm::vec3 look_dir = target_player->transform.position - transform.position;
    look_dir.y = 0.0f;
    look_dir = glm::normalize(look_dir);

    f32 yaw = atan2(look_dir.x, look_dir.z);

    transform.rotation.y = glm::degrees(yaw);
}

void Enemy::Draw(const Shader &shader) const
{
    shader.SetInt("useTexture", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, enemy_tex->ID);

    Entity::Draw(shader);

    shader.SetInt("useTexture", 0);
}
