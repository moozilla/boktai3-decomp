#include "engine/shared/global.h"
#include "engine/shared/engine_globals.h"
#include "engine/shared/ram.h"
s32 Mod(s32, s32);
u8 *Ratatosk_GetRandomMovementPoint(u8 *p)
{
    u32 i;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    i = gRandomIndex;
    return p + (Mod(*(u16 *)((i << 1) + (u32)gRandomTable), 8) * 8 + 0xccc);
}
