#include "system/game_state/game_state.h"
void Inventory_SwapSlots(s32 first, s32 second)
{
    s32 id = *(first + gGameState->table160);
    s32 i;

    *(first + gGameState->table160) = *(second + gGameState->table160);
    *(second + gGameState->table160) = id;
    for (i = 0; i < 4; i++) {
        if (*(i + gGameState->party) == first)
            *(i + gGameState->party) = second;
        else if (*(i + gGameState->party) == second)
            *(i + gGameState->party) = first;
    }
}
