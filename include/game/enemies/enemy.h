#pragma once

#include <game/core/entity.h>
#include <game/core/hurtable.h>
#include <game/player/player.h>

class Enemy : public Entity
{

public:
    Enemy(glm::vec3 spawn_pos, Player *player, Mesh *mesh, Texture *tex);
    virtual ~Enemy() = default;

    virtual void Update(f32 delta_time) override;
    virtual void Draw(const Shader &shader) const override;

private:
    Hurtable health;
    Player *target_player;
    Texture *enemy_tex;

protected:
    f32 move_speed = 3.0f;

};