#include <game/entities/projectile.h>

Projectile::Projectile(glm::vec3 spawn_pos, glm::vec3 velocity,
                       f32 damage, CollisionTag owner_tag,
                       vector<AABB> *level_colliders,
                       vector<Entity *> *entities,
                       Player *player, Mesh *mesh, Texture *tex)
    : Entity(mesh),
      velocity(velocity),
      damage(damage),
      owner_tag(owner_tag),
      level_colliders(level_colliders),
      entities(entities),
      target_player(player),
      proj_texture(tex)
{
    transform.position = spawn_pos;
    transform.scale = glm::vec3(0.15f);
    this->tag = CollisionTag::PROJECTILE;
}

void Projectile::Update(f32 delta_time)
{
    if (pending_destroy)
        return;

    lifetime += delta_time;
    if (lifetime >= MAX_LIFETIME)
    {
        pending_destroy = true;
        return;
    }
    transform.position += velocity * delta_time;

    if (CheckLevelCollision())
    {
        pending_destroy = true;
        return;
    }

    CheckEntityCollision();
}

void Projectile::Draw(const Shader &shader) const
{
    // Bind the texture if we have one
    if (proj_texture)
    {
        shader.SetInt("useTexture", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, proj_texture->ID);
    }
    else
    {
        shader.SetInt("useTexture", 0);
    }

    Entity::Draw(shader);
}

bool Projectile::CheckLevelCollision() const
{
    if (!level_colliders)
        return false;

    glm::vec3 dir = glm::normalize(velocity);
    f32 step_dist = glm::length(velocity) * (1.0f / 60.0f) + 0.15f;

    for (const auto &box : *level_colliders)
    {
        f32 hit_dist = 0.0f;
        glm::vec3 hit_norm;

        if (Physics::RayIntersectAABB(transform.position, dir, box, hit_dist, hit_norm))
        {
            if (hit_dist <= step_dist)
                return true;
        }
    }
    return false;
}

void Projectile::CheckEntityCollision()
{
    f32 hit_threshold = HIT_RADIUS + 0.5f;

    if (target_player && owner_tag != CollisionTag::PLAYER)
    {
        f32 dist_to_player = glm::distance(transform.position, target_player->transform.position);
        if (dist_to_player <= hit_threshold)
        {
            target_player->health.TakeDmg(damage);
            std::cout << "[DEBUG] Player got hit by a snowball! Took " << damage << " damage.\n";
            pending_destroy = true;
            return;
        }
    }

    if (!entities)
        return;

    for (Entity *ent : *entities)
    {
        if (!ent || ent->tag == owner_tag || ent->tag == CollisionTag::PROJECTILE)
            continue;

        f32 dist = glm::distance(transform.position, ent->transform.position);

        if (dist <= hit_threshold)
        {
            if (ent->health)
                ent->health->TakeDmg(damage);

            pending_destroy = true;
            return;
        }
    }
}