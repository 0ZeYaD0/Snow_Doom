#pragma once

#include <engine/core/defines.h>
#include <engine/graphics/texture.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>
using std::string, std::vector;

class Entity;

struct WeaponAnim
{
    vector<i32> frames;
    f32 time_per_frame;
};

class Weapon
{
protected:
    f32 base_dmg;
    f32 max_range;
    f32 fire_rate; // in seconds
    f32 recoil_pitch;

    // ammo & reloading
    i32 max_ammo;
    i32 curr_ammo;
    bool is_reloading;
    f32 reload_time;
    f32 reload_timer;
    
    f32 current_cooldown;

    // visuals
    Texture *weapon_tex;
    i32 total_frames;

    // Animation State
    WeaponAnim anim_idle;
    WeaponAnim anim_fire;
    WeaponAnim anim_reload;
    
    WeaponAnim* current_anim;
    i32 anim_index;
    f32 anim_timer;

public:
    Weapon(f32 dmg, f32 range, f32 rate, f32 recoil, i32 ammo, f32 rld_time, const string& texture_path, i32 frames);
    virtual ~Weapon();

    virtual void Update(f32 delta_time);

    // return true if fired
    virtual bool Fire(glm::vec3 origin, glm::vec3 direction, std::vector<Entity>& entities);

    virtual void Reload();
    int GetCurrentAmmo() const { return curr_ammo; }
    int GetMaxAmmo() const { return max_ammo; }

    f32 GetRecoil() const { return recoil_pitch; }
    i32 GetCurrentFrame() const;

    Texture *GetTexture() const { return weapon_tex; };
    i32 GetTotalFrames() const { return total_frames; };

    bool IsReloading() const { return is_reloading; }
};