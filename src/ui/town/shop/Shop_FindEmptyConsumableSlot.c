#include "system/game_state/game_state.h"
#include "global.h"

s32 Shop_FindEmptyConsumableSlot(const void *unusedOwner)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        if (gGameState->table160[i] < 0)
            return i;
    }
    return -1;
}
