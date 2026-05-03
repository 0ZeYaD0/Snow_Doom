#pragma once
#include "weapon.h"

class AK47 : public Weapon
{
public:
    AK47() : Weapon("AK-47", 35.0f, 2.5f, 150.0f, 0.1f, 0.9f) {}

    void Fire(glm::vec3 origin, glm::vec3 direction) override;
};