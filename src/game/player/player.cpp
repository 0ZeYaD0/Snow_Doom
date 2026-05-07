#include <game/player/player.h>

#include <game/player/weapon/AK47.h>

Player::Player(glm::vec3 spawn_pos)
    : Entity(nullptr), health(100.0f), cam(spawn_pos)
{
    transform.position = spawn_pos;

    this->Entity::health = &this->health;

    current_weapon = new AK47();

    health.on_death = []() -> void
    {
        // TODO: dieee
        return;
    };
}

Player::~Player()
{
    if (current_weapon)
        delete current_weapon;
}

void Player::UpdatePlayer(f32 delta_time, const vector<AABB> &obstacles, vector<Entity> &entities)
{
    if (dash_charges < MAX_DASH_CHARGES)
    {
        dash_recharge_timer += delta_time;

        if (dash_recharge_timer >= DASH_RECHARGE_TIME)
        {
            dash_charges++;
            dash_recharge_timer = 0.0f;
        }
    }
    else
    {
        dash_recharge_timer = 0.0f;
    }

    // mouse
    glm::vec2 mouse_delta = Input::GetMouseDelta();
    cam.ProcessMouseMov(mouse_delta.x, mouse_delta.y);

    // movement
    HandleInput(delta_time, entities);

    // gravity
    if (!is_grounded)
        velocity.y -= 25.0f * delta_time;

    // velocity
    transform.position += velocity * delta_time;

    // resolve collisions
    is_grounded = false;
    for (const auto &obs : obstacles)
    {
        glm::vec3 old_pos = transform.position;

        if (Physics::resolveCollision(transform.position, player_size, obs))
        {
            if (transform.position.y > old_pos.y)
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

void Player::HandleInput(f32 delta_time, vector<Entity> &entities)
{
    // 1. Flatten the camera vectors so looking up/down doesn't slow your movement
    glm::vec3 forward = cam.Front;
    forward.y = 0.0f;
    forward = glm::normalize(forward);

    glm::vec3 right = cam.Right;
    right.y = 0.0f;
    right = glm::normalize(right);

    // 2. Generate wish direction
    glm::vec3 wish_dir = forward * Input::GetAxis("MoveForward") + right * Input::GetAxis("MoveRight");

    if (glm::length(wish_dir) > 0.0f)
        wish_dir = glm::normalize(wish_dir);

    // --- Dash ---
    if (Input::GetActionDown("Dash") && dash_charges > 0 && glm::length(wish_dir) > 0.0f)
    {
        velocity.x = wish_dir.x * DASH_SPEED;
        velocity.z = wish_dir.z * DASH_SPEED;

        if (is_grounded)
        {
            velocity.y = 2.0f;
            is_grounded = false;
        }

        dash_charges--;
        dash_recharge_timer = 0.0f;
    }

    // 3. Auto-Bhop: Changed GetActionDown to GetAction.
    if (Input::GetAction("Jump") && is_grounded)
    {
        velocity.y = 10.0f;
        is_grounded = false;
    }

    // 4. Movement Physics Apply
    if (is_grounded)
    {
        ApplyFriction(delta_time);
        Accelerate(wish_dir, 15.0f, 14.0f, delta_time);
    }
    else
    {
        Accelerate(wish_dir, 15.0f, 2.0f, delta_time);
    }

    // --- pew pew ---
    if (current_weapon)
        current_weapon->UpdateCooldown(delta_time);

    if (Input::GetActionDown("Fire"))
    {
        std::cout << "[INPUT] 'Fire' action triggered!" << std::endl;

        // 2. Checks if the AK47 successfully fired (cooldown is ready, has ammo, etc.)
        if (current_weapon && current_weapon->Fire(cam.position, cam.GetFront(), entities))
        {
            std::cout << "[WEAPON] BANG! AK47 fired. Applying recoil." << std::endl;

            // This is your recoil!
            cam.ProcessMouseMov(0.0f, 5.0f);
        }
        else
        {
            std::cout << "[WEAPON] Click... weapon not ready (cooldown)." << std::endl;
        }
    }
}

void Player::ApplyFriction(f32 dt)
{
    glm::vec3 flat_vel = velocity;
    flat_vel.y = 0.0f;
    f32 speed = glm::length(flat_vel);

    if (speed < 0.1f)
    {
        velocity.x = 0;
        velocity.z = 0;
        return;
    }

    f32 friction = 8.0f;
    f32 drop = speed * friction * dt;
    f32 new_spd = std::max(speed - drop, 0.0f);
    new_spd /= speed;

    velocity.x *= new_spd;
    velocity.z *= new_spd;
}

void Player::Accelerate(glm::vec3 wish_dir, f32 wish_speed, f32 accel, f32 dt)
{
    f32 curr_spd = glm::dot(velocity, wish_dir);
    f32 add_spd = wish_speed - curr_spd;

    if (add_spd <= 0)
        return;

    f32 accel_spd = accel * dt * wish_speed;
    if (accel_spd > add_spd)
        accel_spd = add_spd;

    velocity += accel_spd * wish_dir;
}