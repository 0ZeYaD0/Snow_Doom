#include <game/core/game.h>

/*
    TODO:
    - better input handling
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