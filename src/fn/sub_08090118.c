#include "global.h"
#include "engine_globals.h"

extern u8 gUnk_0203B400[];

u32 Slime_GetRandomVariantFlags(u8 *p)
{
    s32 v;
    u32 r;
    if (*(u16 *)(p + 0x1e6) != 3)
        return 0;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    v = *(u8 *)(gUnk_0203B400 + gRandomIndex * 2);
    r = 0x200004;
    if (v <= 0xb3)
        r = 0x400008;
    return r;
}
