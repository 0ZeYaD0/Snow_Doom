#pragma once
#include <string>
#include <glm/glm.hpp>

using std::string;
class Hurtable;

class Weapon
{
protected:
    string name;

    float basedmg;
    float headshotmult;

    float max_range;
    float dmg_falloff;
    float base_accuracy;

public:
    Weapon(string n, float dmg, float hsMult, float range, float falloff, float acc);
    virtual ~Weapon() = default;

    virtual void Fire(glm::vec3 origin, glm::vec3 dir) = 0;

    float CalculateImpactDmg(float dist, bool is_headshot);

    string GetName() const { return name; }
};