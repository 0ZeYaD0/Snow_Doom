#include <game/entities/pickup.h>

Pickup::Pickup(glm::vec3 spawn_pos, Player *player, Mesh *mesh, Texture *tex, PickupType type, std::string id)
    : Entity(mesh), target_player(player), type(type), item_id(id), pickup_tex(tex)
{
    transform.position = spawn_pos;
    transform.scale = glm::vec3(0.5f);

    initial_y = spawn_pos.y;

    if (type == PickupType::KEY) {
        pickup_sound = new AudioBuffer("res/audio/pickup.ogg");
        audio_source.SetPosition(spawn_pos);
    }
}

void Pickup::Update(f32 delta_time)
{
    if (pending_destroy)
        return;

    float_offset += delta_time * 2.0f;
    transform.position.y = initial_y + sin(float_offset) * 0.2f;
    transform.rotation.y += 45.0f * delta_time;

    glm::vec3 dir_to_player = target_player->transform.position - transform.position;
    f32 dist = glm::length(dir_to_player);

    if (dist < 1.5f)
    {
        if (type == PickupType::KEY)
        {
            target_player->GiveKey(item_id);
            if (pickup_sound) {
                audio_source.Play(pickup_sound);
            }
        }
        else if (type == PickupType::WEAPON)
        {
            target_player->EquipWeapon(item_id);
        }

        pending_destroy = true;
    }
}

void Pickup::Draw(const Shader &shader) const
{
    if (pending_destroy)
        return;

    shader.SetInt("useTexture", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pickup_tex->ID);

    Entity::Draw(shader);

    shader.SetInt("useTexture", 0);
}