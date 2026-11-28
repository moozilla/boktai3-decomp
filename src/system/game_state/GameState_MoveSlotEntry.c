#include "global.h"
#include "system/game_state/game_state.h"
void GameState_MoveSlotEntry(s32 value, s32 slot)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if ((&gGameState->slots88[4])[i] == value)
            (&gGameState->slots88[4])[i] = -1;
    }
    (&gGameState->slots88[4])[slot] = value;
}
