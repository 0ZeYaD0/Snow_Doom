#include <game/core/game.h>

/*
    TODO:
    - better ui
        - dynamic crosshair (WIP)
        - better dash counter
        - gun info UI
        - screen effects
    - sound effects
    - game loop
        - scene management
        - main menu
        - scene workflow (for cutscenes/scene progress/boss figths etc..)
    - interaction system
        - doors open on close proximity to player (automatically)
        - keys for doors
        - picking up weapon
    - enemies
        - enemy types & actions
        - truck-kun boss fight
*/

i32 main()
{
    Game *game = new Game();

    game->Run();

    game->Exit();
}