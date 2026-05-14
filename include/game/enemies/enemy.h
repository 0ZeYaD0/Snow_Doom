#pragma once

#include <game/core/entity.h>
#include <game/core/hurtable.h>
#include <game/player/player.h>

#include <engine/audio/audio_buffer.h>
#include <engine/audio/audio_source.h>

class Enemy : public Entity
{

public:
    Enemy(glm::vec3 spawn_pos, Player *player, Mesh *mesh, Texture *tex);
    virtual ~Enemy() = default;

    virtual void Update(f32 delta_time) override;
    virtual void Draw(const Shader &shader) const override;

    void PlayShootSound();
    void PlayHurtSound();
    void PlayDeathSound();

    AudioBuffer *sfx_death = nullptr;
    AudioBuffer *sfx_shoot = nullptr;
    AudioBuffer *sfx_hurt = nullptr;

private:
    Hurtable health;
    Player *target_player;
    Texture *enemy_tex;
    static const int AUDIO_POOL_SIZE = 4;
    AudioSource audio_pool[AUDIO_POOL_SIZE];
    int current_audio_index = 0;

    void PlaySoundOverlapped(AudioBuffer *buffer);

protected:
    f32 move_speed = 3.0f;
};