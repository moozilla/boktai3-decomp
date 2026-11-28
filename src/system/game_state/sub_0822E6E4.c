#include "global.h"
#include "system/game_state/game_state.h"

bool32 sub_0822E6E4(s32 index)
{
    s32 count = 0;
    s32 i;
    bool32 result;
    for (i = 0; i < 8; i++) {
        if ((&gGameState->inventory[8])[index] > 0)
            count++;
    }
    if (count > 0) {
        (&gGameState->inventory[8])[index] = -1;
        result = TRUE;
    } else {
        result = FALSE;
    }
    return result;
}
