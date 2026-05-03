#include <game/player/weapon/AK47.h>

#include <engine/physics/physics.h>
#include <game/entities/hurtable.h>

#include <iostream>
#include <vector>
using std::vector;

bool AK47::Fire(glm::vec3 origin, glm::vec3 direction, std::vector<Entity>& entities)
{
    vector<Entity *> entity_ptrs;
    for(auto &e : entities)
        entity_ptrs.push_back(&e);

    RaycastHit hit = Physics::Raycast(
        origin,
        direction,
        entity_ptrs,
        max_range
    );

    if(hit.hit && hit.hit_entity)
    {
        if(hit.hit_entity->tag == CollisionTag::ENEMY)
        {
            if(hit.hit_entity->health)
            {
                f32 dmg = CalculateImpactDmg(hit.dist, false);
                hit.hit_entity->health->TakeDmg(dmg);

                std::cout << "pew pew enemy hit" << std::endl;
                return true;
            }
        }
        else if(hit.hit_entity->tag == CollisionTag::ENVIRONMENT)
        {
            std::cout << "pew pew ENV hit" << std::endl;
        }
    }

    return false;
}