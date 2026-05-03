#pragma once

#include <engine/physics/aabb.h>
#include <game/entities/entity.h>

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
    // Checks for a collision and automatically modifies the dynamic object's position to push it out of the static obstacle.
    // Returns true if a collision was resolved.
    static bool resolveCollision(
        glm::vec3 &dynamicPos,
        const glm::vec3 &dynamicSize,
        const AABB &staticObstacle
    );

    static bool RayIntersectAABB(
        glm::vec3 ray_origin,
        glm::vec3 ray_dir,
        const AABB &aabb,
        f32 &out_dist,
        glm::vec3 &out_normal
    );

    static RaycastHit Raycast(
        glm::vec3 ray_origin,
        glm::vec3 ray_dir,
        const vector<Entity *> &entities,
        f32 max_dist
    );
};