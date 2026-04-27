#pragma once

#include <engine/core/defines.h>

class Hurtable
{
private:
    f32 max_hp;
    f32 curr_hp;

public:
    Hurtable(f32 max_hp) : max_hp(max_hp), curr_hp(max_hp) {}
    virtual ~Hurtable() = default;

    virtual void TakeDmg(f32 amount);
    virtual void Heal(f32 amount);

    f32 GetCurrHP();
    f32 GetMaxHP(f32 amount);
    bool IsDead() const { return curr_hp; }

protected:
    virtual void OnDmgTaken(f32 dmg_amount) {}
    virtual void OnDeath() {}
};