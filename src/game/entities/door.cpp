#include <game/entities/door.h>

Door::Door(glm::vec3 spawn_pos, glm::vec3 door_size, Player *player, Mesh *mesh, Texture *tex, std::string req_key)
    : Entity(mesh), target_player(player), required_key(req_key), door_tex(tex), size(door_size)
{
    transform.position = spawn_pos;
    initial_pos = spawn_pos;

    closed_y = transform.position.y;
    open_y = closed_y - size.y;
}

void Door::Update(f32 delta_time)
{
    glm::vec3 dir_to_player = target_player->transform.position - initial_pos;
    f32 dist = glm::length(dir_to_player);

    is_open = (dist < DOOR_OPEN_DIST && (required_key == "none" || target_player->HasKey(required_key)));

    f32 target_y = is_open ? open_y : closed_y;
    transform.position.y = glm::mix(transform.position.y, target_y, swing_speed * delta_time);
}

AABB Door::GetCollider() const
{
    return AABB::fromPosSize(initial_pos, size);
}

void Door::Draw(const Shader &shader) const
{
    shader.SetInt("useTexture", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, door_tex->ID);

    Entity::Draw(shader);

    shader.SetInt("useTexture", 0);
}