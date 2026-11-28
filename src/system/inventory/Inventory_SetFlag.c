#include "system/game_state/game_state.h"

/* Sets bit `id` of the 64-bit flag set in flags720 and flags724. */
void Inventory_SetFlag(u32 id)
{
    if ((s32)id > 31)
        gGameState->flags724 |= 1 << (id - 32);
    else
        gGameState->flags720 |= 1 << id;
}
