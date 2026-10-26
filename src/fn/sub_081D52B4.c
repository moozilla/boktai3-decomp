#include "global.h"

extern u8 *gGameState;

u32 GameState_IsPairFlagSet(u32 a, u32 b)
{
    u32 *p = (u32 *)(gGameState + 0x700);
    u32 idx = (a << 1) + b;
    u32 w = idx >> 5;
    u32 bit;
    if (w > 7) {
        return 0;
    }
    p += w;
    bit = 1 << (idx & 0x1F);
    if (*p & bit) {
        return 1;
    }
    return 0;
}
