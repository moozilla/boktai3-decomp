#include "global.h"
#include "game_state.h"

/* Sets flag 0-82 in the three words at game state +0x6F0 (tentative). */
void GameState_SetEventFlag(u32 flag)
{
    u8 *word;
    u32 mask;
    if (flag > 0x52)
        return;
    if ((s32)flag <= 31) {
        word = gGameState;
        word += 0x6F0;
        mask = 1 << flag;
    } else if ((s32)flag <= 63) {
        word = gGameState;
        word += 0x6F4;
        mask = 1 << (flag - 32);
    } else {
        word = gGameState;
        word += 0x6F8;
        mask = 1 << (flag - 64);
    }
    *(u32 *)word |= mask;
}
