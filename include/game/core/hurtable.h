#pragma once

#include <engine/core/defines.h>

#include <functional>
using std::function;

class Hurtable
{
private:
    f32 max_hp;
    f32 curr_hp;

public:

    function<void()> on_death;
    function<void(f32)> on_dmg_taken;

    Hurtable(f32 max_hp) : max_hp(max_hp), curr_hp(max_hp) {}

    void TakeDmg(f32 amt);
    void Heal(f32 amt);

    f32 GetCurrHP() const { return curr_hp; }
    
    f32 MaxHP(f32 amt = -1) {
        if(amt == -1)
            return max_hp;
        else
            max_hp = amt;

        return -1;
    }

    bool IsDead() const { return curr_hp <= 0.0f; }
};