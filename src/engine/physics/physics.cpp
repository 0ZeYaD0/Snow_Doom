#include <engine/physics/physics.h>
#include <cmath>

#include <algorithm>

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


bool Physics::RayIntersectAABB(glm::vec3 ray_origin, glm::vec3 ray_dir, const AABB &aabb, f32 &out_dist, glm::vec3 &out_normal)
{
    glm::vec3 inv_dir = 1.0f / ray_dir;

    // intersection
    glm::vec3 t0 = (aabb.min - ray_origin) * inv_dir;
    glm::vec3 t1 = (aabb.max - ray_origin) * inv_dir;

    glm::vec3 tmin = glm::min(t0, t1);
    glm::vec3 tmax = glm::max(t0, t1);

    f32 t_near = std::max(std::max(tmin.x, tmin.y), tmin.z);
    f32 t_far  = std::min(std::min(tmax.x, tmax.y), tmax.z);

    if(t_near > t_far || t_far < 0.0f)
        return false;

    out_dist = t_near > 0.0f ? t_near : t_far;

    if(out_dist == tmin.x) out_normal = glm::vec3(-glm::sign(ray_dir.x), 0.0f, 0.0f);
    else if(out_dist == tmin.y) out_normal = glm::vec3(0.0f, -glm::sign(ray_dir.y), 0.0f);
    else out_normal = glm::vec3(0.0f, 0.0f, -glm::sign(ray_dir.z));

    return true;
}

RaycastHit Physics::Raycast(glm::vec3 ray_origin, glm::vec3 ray_dir, const vector<Entity *> &entities, f32 max_dist)
{
    RaycastHit closest_hit;
    closest_hit.dist = max_dist;

    ray_dir = glm::normalize(ray_dir);

    for(Entity *entity : entities)
    {
        if(entity->tag == CollisionTag::DEFAULT) continue;

        AABB entity_bounds = AABB::fromPosSize(entity->transform.position, entity->transform.scale * 2.0f);
        
        f32 hit_dist;
        glm::vec3 hit_normal;

        if(RayIntersectAABB(ray_origin, ray_dir, entity_bounds, hit_dist, hit_normal))
        {
            if(hit_dist < closest_hit.dist && hit_dist >= 0.0f)
            {
                closest_hit.hit = true;
                closest_hit.dist = hit_dist;
                closest_hit.normal = hit_normal;
                closest_hit.point = ray_origin + (ray_dir * hit_dist);
                closest_hit.hit_entity = entity;
            }
        }
    }

    return closest_hit;
}
