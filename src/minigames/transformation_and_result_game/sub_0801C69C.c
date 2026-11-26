#include "minigames/result_game_tutorial/result_tutorial.h"

void ResultGame_SetUpdateCallback(struct ResultMenu *menu, void (*callback)(struct ResultMenu *))
{
    menu->gameUpdate = callback;
    menu->callbackPending = 1;
    menu->unknown788 = 0;
    menu->phase = 0;
    menu->timer = 0;
    menu->elapsed = 0;
}
