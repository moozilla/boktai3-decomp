#include "minigames/result_game/result_tutorial.h"

u32 ResultMenu_IsPhaseStart(struct ResultMenu *menu)
{
    if (menu->phaseStart) {
        menu->phaseStart = 0;
        return 1;
    }
    return 0;
}
