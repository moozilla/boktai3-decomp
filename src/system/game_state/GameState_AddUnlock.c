#include "global.h"
#include "system/game_state/game_state.h"

void GameState_SetUnlockSlot(s32 slot, s32 value); /* sets unlock slot */

/* Puts value in the first free one of the twelve unlock slots (s16 at game
   state +0x4C0); 0 when all are taken. */
u32 GameState_AddUnlock(s32 value)
{
    s32 i;
    for (i = 0; i < 12; i++) {
        if (*(i + gGameState->table4C0) == 0) {
            GameState_SetUnlockSlot(i, value);
            return 1;
        }
    }
    return 0;
}
