#include "global.h"
extern u8 *gUnk_02000488;
s32 sub_0805B28C(u8 *, u8 *, u32);
s32 sub_0805B3A0(u8 *p)
{
    if (sub_0805B28C(gUnk_02000488 + 0x50, p + 0x510, 0x320) != 0) return 0;
    return 1;
}
