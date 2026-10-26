#include "global.h"

extern u8 *gGameState;

u32 GameState_IsEventFlagSet(s32 idx)
{
    u32 *p = (u32 *)(gGameState + 0x6F0);
    s32 w;
    u32 bit;
    if (idx >= 0) {
        w = idx >> 5;
    } else {
        w = -((-idx) >> 5);
    }
    p += w;
    bit = 1 << (idx & 0x1F);
    if (*p & bit) {
        return 1;
    }
    return 0;
}
