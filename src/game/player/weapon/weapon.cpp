#include <game/player/weapon/weapon.h>
#include <engine/physics/physics.h>
#include <algorithm>

#include <iostream>

Weapon::Weapon(f32 dmg, f32 range, f32 rate, f32 recoil, int ammo, f32 rld_time, const std::string& texture_path, int frames)
    : base_dmg(dmg), max_range(range), fire_rate(rate), recoil_pitch(recoil), current_cooldown(0.0f),
      max_ammo(ammo), curr_ammo(ammo), is_reloading(false), reload_time(rld_time), reload_timer(0.0f),
      current_anim(nullptr), anim_index(0), anim_timer(0.0f), total_frames(frames)
{
    weapon_tex = new Texture(texture_path);
}

Weapon::~Weapon()
{
    if(weapon_tex)
        delete weapon_tex;
}

void Weapon::Update(f32 delta_time)
{
    if(hitmarker_timer > 0.0f)
    {
        hitmarker_timer -= delta_time;
    }

    if (is_reloading)
    {
        reload_timer -= delta_time;
        if (reload_timer <= 0.0f)
        {
            is_reloading = false;
            curr_ammo = max_ammo;
            current_anim = &anim_idle;
            anim_index = 0;
        }
    }

    if (current_cooldown > 0.0f)
        current_cooldown -= delta_time;

    
    // animations
    if (current_anim && current_anim->frames.size() > 0)
    {
        anim_timer += delta_time;
        if (anim_timer >= current_anim->time_per_frame)
        {
            anim_timer = 0.0f;
            anim_index++;

            if (anim_index >= current_anim->frames.size())
            {
                if (current_anim != &anim_idle && !is_reloading)
                    current_anim = &anim_idle, anim_index = 0;
                else if (!is_reloading)
                    anim_index = 0; 
                else
                    anim_index = current_anim->frames.size() - 1; 
            }
        }
    }
}

bool Weapon::Fire(glm::vec3 origin, glm::vec3 direction, std::vector<Entity *>& entities)
{
    if (current_cooldown > 0.0f || is_reloading || curr_ammo <= 0)
        return false;

    current_cooldown = fire_rate;
    curr_ammo--;

    current_anim = &anim_fire;
    anim_index = 0;
    anim_timer = 0.0f;

    RaycastHit hit = Physics::Raycast(origin, direction, entities, max_range);

    if(hit.hit && hit.hit_entity && hit.hit_entity->health)
    {
        if(hit.hit_entity->tag == CollisionTag::ENEMY)
        {
            hit.hit_entity->health->TakeDmg(base_dmg);
            hitmarker_timer = 0.15f;
        }
    }

    // auto-reload if firing gun with 0 bullets
    if (curr_ammo <= 0)
    {
        Reload();
    }

    return true;
}

void Weapon::Reload()
{
    if (!is_reloading && curr_ammo < max_ammo)
    {
        is_reloading = true;
        reload_timer = reload_time;
        
        current_anim = &anim_reload;
        anim_index = 0;
        anim_timer = 0.0f;
    }
}

i32 Weapon::GetCurrentFrame() const
{
    if(current_anim && anim_index < current_anim->frames.size())
        return current_anim->frames[anim_index];

    return 0;
}
