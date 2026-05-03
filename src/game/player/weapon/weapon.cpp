#include <game/player/weapon/weapon.h>
#include <algorithm>

Weapon::Weapon(std::string n, float dmg, float hsMult, float range, float falloff, float acc)
    : name(n), basedmg(dmg), headshotmult(hsMult), max_range(range), dmg_falloff(falloff), base_accuracy(acc) {}

float Weapon::CalculateImpactDmg(float dist, bool isHeadshot)
{
    if (dist > max_range)
        return 0.0f;

    float finaldmg = basedmg - (dist * dmg_falloff);

    finaldmg = std::max(0.0f, finaldmg);

    if (isHeadshot)
        finaldmg *= headshotmult;

    return finaldmg;
}