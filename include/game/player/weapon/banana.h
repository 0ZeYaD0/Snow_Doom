#pragma once
#include "weapon.h"

class Banana : public Weapon
{
public:
    Banana()
        : Weapon(30.0f, 1000.0f, 0.1f, 5.0f, 30, 2.0f, "res/art/banana.png", 4)
    {
        anim_idle = { {0}, 1.0f };
        anim_fire = { {1, 2, 3}, 0.2f };

        current_anim = &anim_idle;
    }
};