#include "system/game_state/game_state.h"

u32 Inventory_GetConsumableObtainedFlag(s32 id)
{
    u32 *flags;

    if (id < 32) {
        flags = &gGameState->flags720;
    } else {
        flags = &gGameState->flags724;
        id -= 32;
    }
    return *flags & (1 << id);
}
