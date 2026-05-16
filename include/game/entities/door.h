#pragma once

#include <game/core/entity.h>
#include <game/player/player.h>
#include <engine/physics/aabb.h>
#include <engine/graphics/texture.h>
#include <string>

class Door : public Entity
{
private:
    Player *target_player;
    std::string required_key;
    Texture *door_tex;

    glm::vec3 size;
    glm::vec3 initial_pos;

    f32 closed_yaw = 0.0f;
    f32 open_yaw = 90.0f;

    bool is_open = false;
    f32 swing_speed = 5.0f;

public:
    Door(glm::vec3 spawn_pos, glm::vec3 door_size, Player *player, Mesh *mesh, Texture *tex, std::string req_key);

    void Update(f32 delta_time) override;
    void Draw(const Shader &shader) const override;

    AABB GetCollider() const;
    bool IsOpen() const { return is_open; }
};