#include "global.h"
s32 Mod(s32, s32);
void sub_0806F7B0(u8 *);
u32 sub_0806F838(u8 *p)
{
    if (*(u16 *)(p + 0x24) == 0)
        sub_0806F7B0(p);
    else if (Mod(*(u16 *)(p + 0x26), *(u16 *)(p + 0x24)) == 0)
        sub_0806F7B0(p);
    *(u16 *)(p + 0x26) += 1;
    return 0;
}
