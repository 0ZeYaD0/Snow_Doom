#pragma once

#include <engine/core/defines.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>
using std::string, std::vector;

class Hurtable;
class Entity;

class Weapon
{
protected:
    string name;

    float basedmg;
    float headshotmult;

    float max_range;
    float dmg_falloff;
    float base_accuracy;

    i32 ammo_in_mag;
    i32 mag_size;
    f32 fire_rate;
    f32 current_cooldown;

public:
    Weapon(string n, float dmg, float hsMult, float range, float falloff, float acc, int mag, float rate)
        : name(n), basedmg(dmg), headshotmult(hsMult), max_range(range), dmg_falloff(falloff), 
          base_accuracy(acc), mag_size(mag), ammo_in_mag(mag), fire_rate(rate), current_cooldown(0.0f) {}
          
    virtual ~Weapon() = default;

    // We change Fire to return a boolean so the player knows if a shot actually happened
    virtual bool Fire(glm::vec3 origin, glm::vec3 dir, std::vector<Entity>& entities) = 0;

    // Call this every frame to reduce the cooldown
    void UpdateCooldown(float dt) {
        if (current_cooldown > 0.0f) {
            current_cooldown -= dt;
        }
    }

    float CalculateImpactDmg(float dist, bool is_headshot);
    string GetName() const { return name; }
};