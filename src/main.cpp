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
    - map stuff
        - better map file & loading
        - enemy placement in map file
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