#include "global.h"
#include "system/game_state/game_state.h"

s32 Inventory_GetConsumableId(u32);
u32 GameState_HasEquippedConsumable(s32 id)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (Inventory_GetConsumableId(*(i + gGameState->party)) == id)
            return 1;
    }
    return 0;
}
