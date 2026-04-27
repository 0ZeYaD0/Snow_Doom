#pragma once
#include <glm/glm.hpp>

// AABB is (Axis-Aligned-Bounding-Boxs)
struct AABB
{
    glm::vec3 min;
    glm::vec3 max;

    AABB() : min(0.0f), max(0.0f) {}

    AABB(const glm::vec3 &minVal, const glm::vec3 &maxVal)
        : min(minVal), max(maxVal) {}

    static AABB fromPosSize(const glm::vec3 &centerPos, const glm::vec3 &size)
    {
        glm::vec3 halfSize = size * 0.5f;
        return AABB(centerPos - halfSize, centerPos + halfSize);
    }

    bool overlaps(const AABB &other) const
    {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }
};
