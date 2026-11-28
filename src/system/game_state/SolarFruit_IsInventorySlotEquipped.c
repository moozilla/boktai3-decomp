#include "global.h"
#include "system/game_state/game_state.h"

u32 SolarFruit_IsInventorySlotEquipped(s32 slot)
{
    s32 i = 0;
    s16 *entry = &gGameState->slots88[4];

    for (; i < 8; entry++, i++) {
        if (*entry == slot)
            return 1;
    }
    return 0;
}
