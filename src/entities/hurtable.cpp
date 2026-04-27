#include <entities/hurtable.h>

void Hurtable::TakeDmg(f32 amount)
{
    if (IsDead() || amount <= 0.0f)
        return;

    curr_hp -= amount;

    if (curr_hp <= 0.0f)
    {
        curr_hp = 0.0f;
        OnDeath();
    }
}

void Hurtable::Heal(f32 amount)
{
    if (IsDead() || amount <= 0.0f)
        return;

    if (curr_hp > max_hp)
        curr_hp = max_hp;
}