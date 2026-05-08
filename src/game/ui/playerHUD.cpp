#include <game/ui/playerHUD.h>

PlayerHUD::PlayerHUD()
    :speed_lines_tex(nullptr), crosshair_ring_tex(nullptr)
{
}

PlayerHUD::~PlayerHUD()
{
    if(speed_lines_tex)
        delete speed_lines_tex;

    if(crosshair_ring_tex)
        delete crosshair_ring_tex;
}

void PlayerHUD::Init()
{
    speed_lines_tex = new Texture("res/art/anime_lines.png"); 
    crosshair_ring_tex = new Texture("res/art/crosshair.png");
}

void PlayerHUD::Render(UIManager& ui, const Window& window, const Player* player, f32 delta_time)
{
    if (!player) return;

    DrawSpeedLines(ui, window, player);
    DrawDashCounter(ui, window, player);
    DrawWeapon(ui, window, player);
    DrawCrosshair(ui, window, player, delta_time);
}

void PlayerHUD::DrawSpeedLines(UIManager& ui, const Window& window, const Player* player)
{
    f32 opacity = player->GetSpeedLinesOpacity();
    if (opacity <= 0.01f || !speed_lines_tex) return;

    glm::vec2 pos = glm::vec2(0.0f, 0.0f);
    glm::vec2 size = glm::vec2((f32)window.GetWidth(), (f32)window.GetHeight());
    glm::vec4 tint = glm::vec4(1.0f, 1.0f, 1.0f, opacity);

    ui.DrawSprite(speed_lines_tex, pos, size, 0.0f, tint);
}

void PlayerHUD::DrawWeapon(UIManager& ui, const Window& window, const Player* player)
{
    if (player->current_weapon && player->current_weapon->GetTexture())
    {
        glm::vec2 gun_pos = glm::vec2(
            (window.GetWidth() / 2.0f) - (gun_width / 2.0f) + gun_offset,
            window.GetHeight() - gun_height
        );

        int frame = player->current_weapon->GetCurrentFrame();
        int total_frames = player->current_weapon->GetTotalFrames();
        
        ui.DrawSpriteFrame(
            player->current_weapon->GetTexture(), 
            gun_pos, 
            glm::vec2(gun_width, gun_height), 
            frame, 
            total_frames, 
            glm::vec4(1.0f)
        );
    }
}

void PlayerHUD::DrawDashCounter(UIManager& ui, const Window& window, const Player* player)
{
    f32 box_size = 30.0f;
    f32 padding = 10.0f;
    f32 start_x = 20.0f;
    f32 start_y = window.GetHeight() - 50.0f;

    for (int i = 0; i < player->GetMaxDashCharges(); i++)
    {
        glm::vec2 pos(start_x + (i * (box_size + padding)), start_y);
        glm::vec2 size(box_size, box_size);

        f32 fill = 0.0f;
        if (i < player->GetDashCharges())
            fill = 1.0f;
        else if (i == player->GetDashCharges())
            fill = player->GetDashRechargeTimer() / player->GetDashRechargeTime();

        glm::vec4 color(0.0f, 1.0f, 1.0f, 1.0f);
        glm::vec4 bg_color(0.2f, 0.2f, 0.2f, 1.0f);

        // We use a new generic DrawRect function here!
        ui.DrawRect(pos, size, color, bg_color, fill);
    }
}

void PlayerHUD::DrawCrosshair(UIManager& ui, const Window& window, const Player* player, f32 delta_time)
{
    if (!player->current_weapon) return;

    // --- 1. Animation Logic ---
    int current_ammo = player->current_weapon->GetCurrentAmmo();
    bool is_reloading = player->current_weapon->IsReloading();

    // Detect a shot fired (Punchier animation)
    if (last_ammo_count != -1 && current_ammo < last_ammo_count && !is_reloading)
    {
        crosshair_ring_scale = 1.6f; // Much bigger spike for better impact!
    }
    last_ammo_count = current_ammo;

    // Detect reload start (Clockwise rotation)
    if (is_reloading && !was_reloading)
    {
        target_crosshair_rotation += 90.0f; // Changed to += for clockwise spin
    }
    was_reloading = is_reloading;

    // Smoothly interpolate back to resting states
    crosshair_ring_scale = glm::mix(crosshair_ring_scale, 1.0f, 15.0f * delta_time);
    crosshair_rotation = glm::mix(crosshair_rotation, target_crosshair_rotation, 8.0f * delta_time);

    // --- 2. Rendering Logic ---
    glm::vec2 center(window.GetWidth() / 2.0f, window.GetHeight() / 2.0f);
    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f); 
    glm::vec4 bg_color(0.0f, 0.0f, 0.0f, 0.0f); 

    // Center dot
    f32 dot_size = 4.0f;
    ui.DrawRect(center - glm::vec2(dot_size / 2.0f), glm::vec2(dot_size), color, bg_color, 1.0f);

    // --- Fixed Rectangular Line Math ---
    f32 line_length = 12.0f;
    f32 line_thickness = 2.0f;
    f32 gap = 8.0f; 

    // The distance from the absolute center to the middle of any line
    f32 center_offset = gap + (line_length / 2.0f);

    // Helper to calculate a perfect center-based orbit
    auto get_orbit_pos = [&](f32 offset_x, f32 offset_y, glm::vec2 rect_size) {
        f32 rad = glm::radians(crosshair_rotation);
        f32 c = cos(rad), s = sin(rad);

        // Orbit the center point
        f32 rotated_center_x = offset_x * c - offset_y * s;
        f32 rotated_center_y = offset_x * s + offset_y * c;

        // Get absolute screen position of the center
        glm::vec2 absolute_center = center + glm::vec2(rotated_center_x, rotated_center_y);

        // Subtract half the size to get the top-left corner DrawRect needs
        return absolute_center - (rect_size / 2.0f);
    };

    glm::vec2 vert_size(line_thickness, line_length);
    glm::vec2 horz_size(line_length, line_thickness);

    // Draw the 4 lines using the new orbit math
    ui.DrawRect(get_orbit_pos(0.0f, -center_offset, vert_size), vert_size, color, bg_color, 1.0f, crosshair_rotation); // Top
    ui.DrawRect(get_orbit_pos(0.0f, center_offset, vert_size), vert_size, color, bg_color, 1.0f, crosshair_rotation);  // Bottom
    ui.DrawRect(get_orbit_pos(-center_offset, 0.0f, horz_size), horz_size, color, bg_color, 1.0f, crosshair_rotation); // Left
    ui.DrawRect(get_orbit_pos(center_offset, 0.0f, horz_size), horz_size, color, bg_color, 1.0f, crosshair_rotation);  // Right

    // --- Curved arcs texture ---
    if (crosshair_ring_tex)
    {
        f32 base_ring_size = 30.0f; // Shrunk the base size down!
        f32 actual_ring_size = base_ring_size * crosshair_ring_scale;
        
        glm::vec2 ring_pos = center - glm::vec2(actual_ring_size / 2.0f);
        ui.DrawSprite(crosshair_ring_tex, ring_pos, glm::vec2(actual_ring_size), crosshair_rotation, color);
    }
}