#include "system/game_state/game_state.h"
void Equipment_AssignUniqueSlot(s32 index, s32 id)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (*(i + gGameState->slots88) == id)
            *(i + gGameState->slots88) = -1;
    }
    *(index + gGameState->slots88) = id;
}
