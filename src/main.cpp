#include <game/core/game.h>

/*
    TODO:
    - player death
    - fix ui
    - polish movement
        - dash effects (anime lines, etc..)
        - refactor player variables/state (currently messy)
    - enemies
    - fix weapon fire structure
*/

i32 main()
{
    Game *game = new Game();

    game->Run();

    game->Exit();
}