#pragma once

#include <game/core/entity.h>
#include <game/player/player.h>
#include <engine/graphics/texture.h>
#include <engine/core/input.h>

#include <engine/audio/audio_source.h>
#include <engine/audio/audio_buffer.h>

#include <string>

enum class PickupType
{
    KEY,
    WEAPON
};

class Pickup : public Entity
{
private:
    Player *target_player;
    PickupType type;
    std::string item_id;
    Texture *pickup_tex;

    f32 float_offset = 0.0f;

    f32 initial_y = 0.0f;
    AudioSource audio_source;
    AudioBuffer* pickup_sound = nullptr;

public:
    Pickup(glm::vec3 spawn_pos, Player *player, Mesh *mesh, Texture *tex, PickupType type, std::string id);

    void Update(f32 delta_time) override;
    void Draw(const Shader &shader) const override;
};