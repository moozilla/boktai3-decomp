#include "global.h"

#define GameState_SetPairFlag sub_081D5188
extern u8 *gUnk_02000710;
/* Sets bit a * 2 + b of the flag words at game state +0x700 (tentative). */
void GameState_SetPairFlag(s32 a, s32 b)
{
    u32 bit;
    u32 *w = (u32 *)(gUnk_02000710 + 0x700);
    bit = a * 2 + b;
    w += bit >> 5;
    *w |= 1 << (bit & 31);
}
