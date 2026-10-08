#include "global.h"
extern u32 gUnk_03005308;
extern u8 gUnk_0203B400[];
s32 Mod(s32, s32);
u8 *sub_081F30F0(u8 *p)
{
    u32 i;
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
    i = gUnk_03005308;
    return p + (Mod(*(u16 *)((i << 1) + (u32)gUnk_0203B400), 8) * 8 + 0xccc);
}
