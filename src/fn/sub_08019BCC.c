#include "global.h"
extern u32 gUnk_020000B0;
void sub_08019BCC(u8 *p, u32 v)
{
    if (gUnk_020000B0 != 0)
        *(u16 *)(p + 0x1c) = v;
}
