#pragma once

#include <engine/ui/ui_manager.h>
#include <engine/graphics/texture.h>
#include <game/player/player.h>
#include <engine/window/window.h>

class PlayerHUD
{
private:
    Texture *speed_lines_tex;
    Texture* crosshair_ring_tex;

    void DrawDashCounter(UIManager& ui, const Window& window, const Player* player);
    void DrawWeapon(UIManager& ui, const Window& window, const Player* player);
    void DrawSpeedLines(UIManager& ui, const Window& window, const Player* player);
    void DrawCrosshair(UIManager& ui, const Window& window, const Player* player, f32 delta_time);

public:
    PlayerHUD();
    ~PlayerHUD();

    void Init();
    void Render(UIManager &ui, const Window &window, const Player *player, f32 delta_time);

private:
    // --- gun ---
    f32 gun_width = 400.0f;
    f32 gun_height = 400.0f;
    f32 gun_offset = 150.0f;

    // --- crosshair ---
    f32 crosshair_rotation = 0.0f;
    f32 target_crosshair_rotation = 0.0f;
    f32 crosshair_ring_scale = 1.0f;
    
    i32 last_ammo_count = -1;
    bool was_reloading = false;
};