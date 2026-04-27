#include <game/entities/hurtable.h>

void Hurtable::TakeDmg(f32 amt)
{
    if (IsDead() || amt <= 0.0f)
        return;

    curr_hp -= amt;

    if(on_dmg_taken)
        on_dmg_taken(amt);
    
    if(curr_hp <= 0.0f) {
        curr_hp = 0.0f;
        if(on_death)
            on_death();
    }
}

void Hurtable::Heal(f32 amount)
{
    if (IsDead() || amount <= 0.0f)
        return;

    curr_hp += amount;

    if (curr_hp > max_hp)
        curr_hp = max_hp;
}