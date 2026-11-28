#include "global.h"
#include "system/game_state/game_state.h"

u32 GameState_IsConsumableSlotEquipped(s32 slot)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (gGameState->party[i] == slot)
            return 1;
    }
    return 0;
}
