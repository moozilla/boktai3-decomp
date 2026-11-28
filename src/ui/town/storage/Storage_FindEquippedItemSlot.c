#include "global.h"
#include "system/game_state/game_state.h"
s32 Storage_FindEquippedItemSlot(u32 unused, s32 slot)
{
    s32 i;
    if (slot > 0xf)
        return -1;
    for (i = 0; i < 8; i++) {
        if (gGameState->inventory[i] == slot)
            return i;
    }
    return -1;
}
