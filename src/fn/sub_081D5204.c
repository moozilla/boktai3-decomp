#include "global.h"

extern u8 *gUnk_02000710;
/* Sets flag 0-82 in the three words at game state +0x6F0 (tentative). */
void GameState_SetEventFlag(u32 flag)
{
    u8 *word;
    u32 mask;
    if (flag > 0x52)
        return;
    if ((s32)flag <= 31) {
        word = gUnk_02000710;
        word += 0x6F0;
        mask = 1 << flag;
    } else if ((s32)flag <= 63) {
        word = gUnk_02000710;
        word += 0x6F4;
        mask = 1 << (flag - 32);
    } else {
        word = gUnk_02000710;
        word += 0x6F8;
        mask = 1 << (flag - 64);
    }
    *(u32 *)word |= mask;
}
