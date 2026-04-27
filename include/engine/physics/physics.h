#pragma once

#include <engine/physics/aabb.h>
#include <glm/glm.hpp>

class Physics
{
public:
    // Checks for a collision and automatically modifies the dynamic object's position to push it out of the static obstacle.
    // Returns true if a collision was resolved.
    static bool resolveCollision(glm::vec3 &dynamicPos, const glm::vec3 &dynamicSize, const AABB &staticObstacle);
};