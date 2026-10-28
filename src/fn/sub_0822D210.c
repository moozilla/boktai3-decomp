#include "global.h"
#include "engine_globals.h"
extern u8 gUnk_0203B400[];
s32 Mod(s32, s32);
u32 Inventory_RollIcePopStick(void)
{
    s32 m;
    u32 i = (gRandomIndex + 1) & 0x3ff;
    gRandomIndex = i;
    m = Mod(*(u16 *)((u8 *)(i * 2) + (u32)gUnk_0203B400), 100);
    if (m <= 1)
        return 0x1f;
    return 0x1e;
}
