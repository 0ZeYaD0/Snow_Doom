#include <game/entities/player.h>

Player::Player(glm::vec3 spawn_pos)
    : Entity(nullptr), health(100.0f), cam(spawn_pos)
{
    transform.position = spawn_pos;

    health.on_death = []() -> void {
        // TODO: dieee
        return;
    };
}

void Player::UpdatePlayer(f32 delta_time, const vector<AABB> &obstacles)
{
    // mouse
    glm::vec2 mouse_delta = Input::GetMouseDelta();
    cam.ProcessMouseMov(mouse_delta.x, mouse_delta.y);

    // movement
    HandleInput(delta_time);

    // gravity
    if(!is_grounded)
        velocity.y -= 25.0f * delta_time;

    // velocity
    transform.position += velocity * delta_time;

    // resolve collisions
    is_grounded = false;
    for(const auto &obs : obstacles)
    {
        glm::vec3 old_pos = transform.position;

        if(Physics::resolveCollision(transform.position, player_size, obs))
        {
            if(transform.position.y > old_pos.y)
            {
                is_grounded = true;
                velocity.y = 0.0f;
            }
            else if (transform.position.y < old_pos.y && velocity.y > 0.0f)
            {
                velocity.y = 0.0f;
            }
        }
    }

    // cam
    cam.position = transform.position + glm::vec3(0.0f, 0.8f, 0.0f);
}

void Player::HandleInput(f32 delta_time)
{
    glm::vec3 wish_dir(0.0f);

    if (Input::GetKey(GLFW_KEY_W)) wish_dir += cam.Front;
    if (Input::GetKey(GLFW_KEY_S)) wish_dir -= cam.Front;
    if (Input::GetKey(GLFW_KEY_D)) wish_dir += cam.Right;
    if (Input::GetKey(GLFW_KEY_A)) wish_dir -= cam.Right;

    wish_dir.y = 0.0f;
    if(glm::length(wish_dir) > 0.0f)
        wish_dir = glm::normalize(wish_dir);

    if (Input::GetKeyDown(GLFW_KEY_SPACE) && is_grounded)
    {
        velocity.y = 10.0f;
        is_grounded = false;
    }

    if (is_grounded)
    {
        ApplyFriction(delta_time);
        Accelerate(wish_dir, 15.0f, 10.0f, delta_time);
    }
    else
    {
        Accelerate(wish_dir, 2.0f, 2.0f, delta_time);
    }

}

void Player::ApplyFriction(f32 dt)
{
    glm::vec3 flat_vel = velocity;
    flat_vel.y = 0.0f;
    f32 speed = glm::length(flat_vel);

    if(speed < 0.1f) {
        velocity.x = 0, velocity.z = 0;
        return;
    }

    f32 drop = speed * 6.0 * dt;
    f32 new_spd = std::max(speed - drop, 0.0f);
    new_spd /= speed;

    velocity.x *= new_spd;
    velocity.z *= new_spd;
}

void Player::Accelerate(glm::vec3 wish_dir, f32 wish_speed, f32 accel, f32 dt)
{
    f32 curr_spd = glm::dot(velocity, wish_dir);
    f32 add_spd  = wish_speed - curr_spd;

    if(add_spd <= 0) return;

    f32 accel_spd = accel * dt * wish_speed;
    if(accel_spd > add_spd) accel_spd = add_spd;

    velocity += accel_spd * wish_dir;
}
