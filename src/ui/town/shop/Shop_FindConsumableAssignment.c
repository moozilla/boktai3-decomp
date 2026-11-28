#include "system/game_state/game_state.h"
#include "global.h"

s32 Shop_FindConsumableAssignment(s32 slot)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        if (gGameState->party[i] == slot)
            return i;
    }
    return -1;
}
