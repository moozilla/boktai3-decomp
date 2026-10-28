#include "global.h"
#include "engine_globals.h"
extern u8 gUnk_0203B400[];
s32 Mod(s32, s32);
u8 *Ratatosk_GetRandomMovementPoint(u8 *p)
{
    u32 i;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    i = gRandomIndex;
    return p + (Mod(*(u16 *)((i << 1) + (u32)gUnk_0203B400), 8) * 8 + 0xccc);
}
