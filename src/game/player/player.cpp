#include <game/player/player.h>

#include <game/player/weapon/banana.h>

Player::Player(glm::vec3 spawn_pos)
    : Entity(nullptr), health(100.0f), cam(spawn_pos)
{
    transform.position = spawn_pos;
    this->Entity::health = &this->health;
    current_weapon = new Banana();

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

void Player::UpdatePlayer(f32 delta_time, const vector<AABB> &obstacles, vector<Entity *> &entities)
{
    for (int i = 0; i < AUDIO_POOL_SIZE; i++)
        audio_pool[i].SetPosition(transform.position);
    UpdateTimers(delta_time);

    UpdateMouseLook();
    HandleInput(delta_time, entities);

    UpdatePhysics(delta_time, obstacles);

    UpdateCameraEffects(delta_time);
}

void Player::UpdateTimers(f32 delta_time)
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

    if (current_weapon)
    {
        current_weapon->Update(delta_time);
    }
}

void Player::UpdateMouseLook()
{
    glm::vec2 mouse_delta = Input::GetMouseDelta();
    cam.ProcessMouseMov(mouse_delta.x, mouse_delta.y);
}

void Player::HandleInput(f32 delta_time, vector<Entity *> &entities)
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
        Accelerate(wish_dir, MAX_GROUND_SPEED, GROUND_ACCEL, delta_time);
    }
    else
    {
        Accelerate(wish_dir, MAX_AIR_SPEED, AIR_ACCEL, delta_time);
    }

    // --- pew pew ---
    if (current_weapon)
    {
        if (Input::GetAction("Fire"))
        {
            if (current_weapon->Fire(cam.position, cam.Front, entities))
            {
                cam.ProcessMouseMov(0.0f, current_weapon->GetRecoil());
                PlayShootSound();
            }
        }

        if (Input::GetActionDown("Reload"))
            current_weapon->Reload();
    }
}

void Player::UpdatePhysics(f32 delta_time, const vector<AABB> &obstacles)
{
    if (!is_grounded)
        velocity.y -= GRAVITY_MULT * delta_time;

    transform.position += velocity * delta_time;

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
}

void Player::UpdateCameraEffects(f32 delta_time)
{
    UpdateCameraTilt(delta_time);

    if (head_bob)
        HeadBob(delta_time);
    else
        cam.position = transform.position + glm::vec3(0.0f, CAM_Y_POS, 0.0f);

    UpdateSpeedLines(delta_time);
}

void Player::ApplyFriction(f32 dt)
{
    glm::vec3 flat_vel = velocity;
    flat_vel.y = 0.0f;
    f32 speed = glm::length(flat_vel);

    if (speed < STOP_SPEED_THRESHOLD)
    {
        velocity.x = 0;
        velocity.z = 0;
        return;
    }

    f32 drop = speed * GROUND_FRICTION * dt;
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

void Player::UpdateCameraTilt(f32 delta_time)
{
    f32 move_right = Input::GetAxis("MoveRight");
    f32 target_roll = 0.0f;

    if (move_right != 0.0f)
        target_roll = move_right * CAM_MAX_TILT;

    f32 t = glm::clamp(CAM_TILT_SPEED * delta_time, 0.0f, 1.0f);
    cam.Roll = glm::mix(cam.Roll, target_roll, t);
    cam.UpdateCameraVectors();
}

void Player::HeadBob(f32 delta_time)
{
    glm::vec3 flat_vel = velocity;
    flat_vel.y = 0.0f;
    f32 spd = glm::length(flat_vel);

    f32 target_amp = 0.0f;
    f32 target_freq = 0.0f;

    if (spd > BOB_MIN_SPEED)
    {
        target_amp = BOB_MOVE_AMP;
        target_freq = BOB_MOVE_FREQ;
    }
    else
    {
        target_amp = BOB_IDLE_AMP;
        target_freq = BOB_IDLE_FREQ;
    }

    if (!is_grounded && abs(velocity.y) > FALL_VELOCITY_THRESHOLD)
        target_amp = 0.0f;

    current_bob_amp = glm::mix(current_bob_amp, target_amp, BOB_SMOOTHING * delta_time);
    bob_phase += target_freq * delta_time;

    if (bob_phase > 2 * glm::pi<f32>())
        bob_phase -= 2 * glm::pi<f32>();

    f32 bob_offset_y = sin(bob_phase) * current_bob_amp;

    cam.position = transform.position + glm::vec3(0.0f, CAM_Y_POS + bob_offset_y, 0.0f);
}

void Player::UpdateSpeedLines(f32 delta_time)
{
    glm::vec3 flat_vel = velocity;
    flat_vel.y = 0.0f;
    f32 current_speed = glm::length(flat_vel);

    f32 target_opacity = 0.0f;

    if (current_speed > MAX_GROUND_SPEED + 2.0f)
    {
        target_opacity = (current_speed - MAX_GROUND_SPEED) / (DASH_SPEED - MAX_GROUND_SPEED);
        target_opacity = glm::clamp(target_opacity, 0.0f, 1.0f);
    }

    speed_lines_opacity = glm::mix(speed_lines_opacity, target_opacity, 8.0f * delta_time);
}

void Player::PlaySoundOverlapped(AudioBuffer *buffer)
{
    if (!buffer)
        return;

    audio_pool[current_audio_index].Play(buffer);

    current_audio_index = (current_audio_index + 1) % AUDIO_POOL_SIZE;
}

void Player::PlayShootSound()
{
    PlaySoundOverlapped(sfx_shoot);
}

void Player::GiveKey(const string &key_id)
{
    inventory_keys.push_back(key_id);
}

bool Player::HasKey(const string &key_id)
{
    for (const auto &key : inventory_keys)
    {
        if (key == key_id)
            return true;
    }
    return false;
}

void Player::EquipWeapon(const string &weapon_id)
{
    if (current_weapon != nullptr)
    {
        delete current_weapon;
        current_weapon = nullptr;
    }

    if (weapon_id == "banana_gun")
    {
        current_weapon = new Banana();
    }

    else
        std::cout << "WTF THERE IS ME IF U KNOW U KNOW \n";
}