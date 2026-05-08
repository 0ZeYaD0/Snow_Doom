#include <game/core/game.h>

/*
    TODO:
    - better ui
        - dynamic crosshair (WIP)
        - better dash counter
        - gun info UI
        - screen effects
    - polish movement
        - dash effects (WIP)
    - sound effects
    - player death
    - enemies
    - map
*/

i32 main()
{
    Game *game = new Game();

    game->Run();

    game->Exit();
}