#include <physics/physics.h>
#include <cmath>

bool Physics::resolveCollision(glm::vec3 &dynamicPos, const glm::vec3 &dynamicSize, const AABB &staticObstacle)
{
    AABB dynamicBox = AABB::fromPosSize(dynamicPos, dynamicSize);

    if (!dynamicBox.overlaps(staticObstacle))
    {
        return false;
    }

    glm::vec3 centerDyn = (dynamicBox.min + dynamicBox.max) * 0.5f;
    glm::vec3 centerStat = (staticObstacle.min + staticObstacle.max) * 0.5f;

    glm::vec3 extentsDyn = (dynamicBox.max - dynamicBox.min) * 0.5f;
    glm::vec3 extentsStat = (staticObstacle.max - staticObstacle.min) * 0.5f;

    glm::vec3 distance = centerDyn - centerStat;
    glm::vec3 minDistance = extentsDyn + extentsStat;

    float penX = minDistance.x - std::abs(distance.x);
    float penY = minDistance.y - std::abs(distance.y);
    float penZ = minDistance.z - std::abs(distance.z);

    if (penX < penY && penX < penZ)
    {
        dynamicPos.x += glm::sign(distance.x) * penX;
    }
    else if (penY < penX && penY < penZ)
    {
        dynamicPos.y += glm::sign(distance.y) * penY;
    }
    else
    {
        dynamicPos.z += glm::sign(distance.z) * penZ;
    }

    return true;
}