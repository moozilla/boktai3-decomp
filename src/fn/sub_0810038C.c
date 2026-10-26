#include "global.h"
extern u8 *gGameState;

static inline u8 Eq(u32 a, u32 b)
{
    if (a == b)
        return TRUE;
    return FALSE;
}

u32 FloorSwitch_UpdateRegionVisibility(u32 *p)
{
    u32 v = p[0x54 / 4];
    if (Eq(*(u32 *)(gGameState + 0x5A0), v)) {
        *p &= ~1;
        return 1;
    }
    *p |= 1;
    return 0;
}
