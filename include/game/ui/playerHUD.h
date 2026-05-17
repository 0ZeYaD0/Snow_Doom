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
    void DrawHealthBar(UIManager& ui, const Window& window, const Player* player);
    void DrawAmmo(UIManager& ui, const Window& window, const Player* player);

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
    
    // New dynamic gap tracker
    f32 current_crosshair_gap = 8.0f; 

    int last_ammo_count = -1;
    bool was_reloading = false;

    // --- Configuration Constants ---
    const f32 CROSSHAIR_DOT_SIZE = 5.0f;
    const f32 CROSSHAIR_LINE_LENGTH = 12.0f;
    const f32 CROSSHAIR_LINE_THICKNESS = 3.0f;
    const f32 CROSSHAIR_BASE_GAP = 8.0f;
    const f32 CROSSHAIR_MAX_GAP = 24.0f; 
    const f32 CROSSHAIR_RING_BASE_SIZE = 45.0f;
    
    // crosshair animation constants
    const f32 CROSSHAIR_EXPAND_SPEED = 15.0f; 
    const f32 CROSSHAIR_SMOOTHING = 15.0f;
    const f32 CROSSHAIR_RING_SHOOT_SCALE = 2.2f;
    const f32 CROSSHAIR_RELOAD_ROTATION = 90.0f;
    const f32 CROSSHAIR_SCALE_SMOOTHING = 15.0f;
    const f32 CROSSHAIR_ROT_SMOOTHING = 8.0f;
};