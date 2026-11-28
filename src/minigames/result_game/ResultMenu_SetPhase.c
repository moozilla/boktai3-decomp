#include "minigames/result_game/result_tutorial.h"

void ResultMenu_SetPhase(struct ResultMenu *menu, void (*update)(void))
{
    menu->phaseFrame = 0;
    menu->phaseStart = 1;
    menu->update = update;
}
