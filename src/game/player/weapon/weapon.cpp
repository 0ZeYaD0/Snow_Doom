#include <game/player/weapon/weapon.h>
#include <algorithm>

float Weapon::CalculateImpactDmg(float dist, bool is_headshot)
{
    float dropoff_factor = std::max(0.0f, 1.0f - (dist / max_range) * dmg_falloff);
    float final_dmg = basedmg * dropoff_factor;
    
    if (is_headshot) final_dmg *= headshotmult;
    
    return final_dmg;
}