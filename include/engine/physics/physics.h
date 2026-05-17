#pragma once

#include <engine/physics/aabb.h>
#include <game/core/entity.h>
#include <game/core/hurtable.h>

#include <glm/glm.hpp>

#include <vector>
using std::vector;

struct RaycastHit
{
    bool hit = false;
    f32 dist = -1.0f;

    glm::vec3 point = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);

    Entity *hit_entity = nullptr;
};

class Physics
{
public:
    static bool resolveCollision(
        glm::vec3 &dynamicPos,
        const glm::vec3 &dynamicSize,
        const AABB &staticObstacle);

    static bool RayIntersectAABB(
        glm::vec3 ray_origin,
        glm::vec3 ray_dir,
        const AABB &aabb,
        f32 &out_dist,
        glm::vec3 &out_normal);

    static RaycastHit Raycast(
        glm::vec3 ray_origin,
        glm::vec3 ray_dir,
        const vector<Entity *> &entities,
        f32 max_dist);
};