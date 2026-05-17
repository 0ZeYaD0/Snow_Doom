#pragma once

#include <game/core/entity.h>
#include <engine/physics/physics.h>
#include <engine/physics/aabb.h>
#include <game/player/player.h>
#include <engine/graphics/texture.h>

#include <vector>
using std::vector;

class Projectile : public Entity
{
public:
    Projectile(glm::vec3 spawn_pos,
               glm::vec3 velocity,
               f32 damage,
               CollisionTag owner_tag,
               vector<AABB> *level_colliders,
               vector<Entity *> *entities,
               Player *player,
               Mesh *mesh, Texture *tex);

    ~Projectile() = default;

    void Update(f32 delta_time) override;
    void Draw(const Shader &shader) const override;

private:
    glm::vec3 velocity;
    f32 damage;
    CollisionTag owner_tag;
    Player *target_player;
    Texture *proj_texture;

    f32 lifetime = 0.0f;
    const f32 MAX_LIFETIME = 6.0f;
    const f32 HIT_RADIUS = 0.55f;

    vector<AABB> *level_colliders;
    vector<Entity *> *entities;

    bool CheckLevelCollision() const;
    void CheckEntityCollision();
};