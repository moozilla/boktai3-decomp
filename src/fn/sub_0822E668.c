#include "global.h"

extern u8 *gGameState;
void GameState_SetUnlockSlot(s32 slot, s32 value); /* sets unlock slot */

/* Puts value in the first free one of the twelve unlock slots (s16 at game
   state +0x4C0); 0 when all are taken. */
u32 GameState_AddUnlock(s32 value)
{
    s32 i;
    for (i = 0; i < 12; i++) {
        if (*(s16 *)(i * 2 + gGameState + 0x4C0) == 0) {
            GameState_SetUnlockSlot(i, value);
            return 1;
        }
    }
    return 0;
}
