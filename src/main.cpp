#include <game/core/game.h>

/*
    TODO:
    - player death
    - fix ui
    - polish movement
        - dash effects (anime lines, etc..)
        - camera tilt
        - head bob
    - enemies
    - fix weapon fire structure
*/

i32 main()
{
    Game *game = new Game();

    game->Run();

    game->Exit();
}