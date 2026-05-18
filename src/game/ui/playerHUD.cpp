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
    death_tex = new Texture("res/art/you_died.png");
}

void PlayerHUD::Render(UIManager& ui, const Window& window, const Player* player, f32 delta_time)
{
    if (!player) return;

    if (player->IsDead())
    {
        DrawDeathScreen(ui, window);
        return;
    }

    DrawSpeedLines(ui, window, player);
    DrawDashCounter(ui, window, player);
    DrawWeapon(ui, window, player);
    DrawCrosshair(ui, window, player, delta_time);
    DrawHealthBar(ui, window, player);
    DrawAmmo(ui, window, player);
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
    int max_dashes = player->GetMaxDashCharges();
    int current_dashes = player->GetDashCharges();
    
    f32 recharge_pct = player->GetDashRechargeTimer() / player->GetDashRechargeTime(); 

    glm::vec2 size(40.0f, 15.0f);
    
    f32 health_bar_height = 30.0f;
    f32 bottom_margin = 40.0f;
    f32 gap = 15.0f; 
    
    f32 base_y = window.GetHeight() - bottom_margin - health_bar_height - gap - size.y;
    glm::vec2 start_pos(40.0f, base_y); 
    
    glm::vec4 active_col(0.9f, 0.9f, 0.9f, 1.0f); 
    glm::vec4 bg_col(0.2f, 0.2f, 0.2f, 0.5f);  
    
    for(int i = 0; i < max_dashes; i++)
    {
        f32 fill = 0.0f;
        if (i < current_dashes) fill = 1.0f;
        else if (i == current_dashes) fill = recharge_pct;

        ui.DrawRect(start_pos + glm::vec2(i * (size.x + 10.0f), 0.0f), size, active_col, bg_col, fill);
    }
}

void PlayerHUD::DrawCrosshair(UIManager& ui, const Window& window, const Player* player, f32 delta_time)
{
    if (!player->current_weapon) return;

    // --- 1. Animation Logic ---
    int current_ammo = player->current_weapon->GetCurrentAmmo();
    bool is_reloading = player->current_weapon->IsReloading();

    // Detect a shot fired
    if (last_ammo_count != -1 && current_ammo < last_ammo_count && !is_reloading)
    {
        crosshair_ring_scale = CROSSHAIR_RING_SHOOT_SCALE; 
    }
    last_ammo_count = current_ammo;

    // Detect reload start (Clockwise rotation)
    if (is_reloading && !was_reloading)
    {
        target_crosshair_rotation += CROSSHAIR_RELOAD_ROTATION; 
    }
    was_reloading = is_reloading;

    // --- Velocity Gap Logic ---
    glm::vec3 flat_vel = player->velocity;
    flat_vel.y = 0.0f;
    f32 current_speed = glm::length(flat_vel);
    
    // Create a ratio based on how fast we are moving
    f32 speed_ratio = current_speed / CROSSHAIR_EXPAND_SPEED;
    speed_ratio = glm::clamp(speed_ratio, 0.0f, 1.2f); 

    // Calculate where the gap *should* be right now
    f32 target_gap = CROSSHAIR_BASE_GAP + (speed_ratio * (CROSSHAIR_MAX_GAP - CROSSHAIR_BASE_GAP));

    // Smoothly interpolate all visual states
    crosshair_ring_scale = glm::mix(crosshair_ring_scale, 1.0f, CROSSHAIR_SCALE_SMOOTHING * delta_time);
    crosshair_rotation = glm::mix(crosshair_rotation, target_crosshair_rotation, CROSSHAIR_ROT_SMOOTHING * delta_time);
    current_crosshair_gap = glm::mix(current_crosshair_gap, target_gap, CROSSHAIR_SMOOTHING * delta_time);

    // --- 2. Rendering Logic ---
    glm::vec2 center(window.GetWidth() / 2.0f, window.GetHeight() / 2.0f);
    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f); 
    glm::vec4 bg_color(0.0f, 0.0f, 0.0f, 0.0f); 

    if (player->current_weapon->WasHitRecently())
    {
        // Paint it all violently red
        color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); 
    }

    // Center dot
    ui.DrawRect(center - glm::vec2(CROSSHAIR_DOT_SIZE / 2.0f), glm::vec2(CROSSHAIR_DOT_SIZE), color, bg_color, 1.0f);

    // Calculate center offset using the dynamic gap
    f32 center_offset = current_crosshair_gap + (CROSSHAIR_LINE_LENGTH / 2.0f);

    // Helper to calculate a perfect center-based orbit
    auto get_orbit_pos = [&](f32 offset_x, f32 offset_y, glm::vec2 rect_size) {
        f32 rad = glm::radians(crosshair_rotation);
        f32 c = cos(rad), s = sin(rad);

        f32 rotated_center_x = offset_x * c - offset_y * s;
        f32 rotated_center_y = offset_x * s + offset_y * c;

        glm::vec2 absolute_center = center + glm::vec2(rotated_center_x, rotated_center_y);
        return absolute_center - (rect_size / 2.0f);
    };

    glm::vec2 vert_size(CROSSHAIR_LINE_THICKNESS, CROSSHAIR_LINE_LENGTH);
    glm::vec2 horz_size(CROSSHAIR_LINE_LENGTH, CROSSHAIR_LINE_THICKNESS);

    // Draw the 4 rotating lines
    ui.DrawRect(get_orbit_pos(0.0f, -center_offset, vert_size), vert_size, color, bg_color, 1.0f, crosshair_rotation); 
    ui.DrawRect(get_orbit_pos(0.0f, center_offset, vert_size), vert_size, color, bg_color, 1.0f, crosshair_rotation);  
    ui.DrawRect(get_orbit_pos(-center_offset, 0.0f, horz_size), horz_size, color, bg_color, 1.0f, crosshair_rotation); 
    ui.DrawRect(get_orbit_pos(center_offset, 0.0f, horz_size), horz_size, color, bg_color, 1.0f, crosshair_rotation);  

    // Curved arcs texture
    if (crosshair_ring_tex)
    {
        f32 actual_ring_size = CROSSHAIR_RING_BASE_SIZE * crosshair_ring_scale;
        glm::vec2 ring_pos = center - glm::vec2(actual_ring_size / 2.0f);
        ui.DrawSprite(crosshair_ring_tex, ring_pos, glm::vec2(actual_ring_size), crosshair_rotation, color);
    }
}

void PlayerHUD::DrawHealthBar(UIManager& ui, const Window& window, const Player* player)
{
    f32 health_pct = player->GetHealth() / player->GetMaxHealth(); 
    
    glm::vec2 size(300.0f, 30.0f);
    glm::vec2 pos(40.0f, window.GetHeight() - size.y - 40.0f); 
    
    glm::vec4 bg_color(0.1f, 0.1f, 0.1f, 0.8f);
    glm::vec4 hp_color(0.7f, 0.1f, 0.1f, 1.0f);
    
    ui.DrawRect(pos, size, hp_color, bg_color, health_pct);
}

void PlayerHUD::DrawAmmo(UIManager& ui, const Window& window, const Player* player)
{
    Weapon* weapon = player->GetActiveWeapon();
    if (!weapon) return;
    
    int current_ammo = weapon->GetCurrentAmmo();
    int mag_size = weapon->GetMaxAmmo();
    
    glm::vec2 bullet_size(25.0f, 8.0f);
    
    glm::vec2 start_pos(window.GetWidth() - 40.0f - bullet_size.x, window.GetHeight() - 40.0f - bullet_size.y);
    
    glm::vec4 bullet_col(0.8f, 0.6f, 0.1f, 1.0f); 
    glm::vec4 spent_col(0.15f, 0.15f, 0.15f, 0.6f);
    
    for(int i = 0; i < mag_size; i++)
    {
        glm::vec4 col = (i < current_ammo) ? bullet_col : spent_col;
        
        ui.DrawRect(start_pos - glm::vec2(0.0f, i * (bullet_size.y + 5.0f)), bullet_size, col, glm::vec4(0.0f), 1.0f);
    }
}

void PlayerHUD::DrawDeathScreen(UIManager& ui, const Window& window)
{
    if (!death_tex) return;

    glm::vec2 size(window.GetWidth(), window.GetWidth() / 4.0f);
    glm::vec2 pos((window.GetWidth() - size.x) / 2.0f, (window.GetHeight() - size.y) / 2.0f);
    
    ui.DrawSprite(death_tex, pos, size, 0.0f, glm::vec4(1.0f));
}