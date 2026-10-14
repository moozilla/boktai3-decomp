#include "global.h"

extern u8 *gUnk_03006A80;
void sub_080483AC(u8 *, u8);

void sub_08048438(u8 *p)
{
    u8 *g = gUnk_03006A80;
    if (g[0] == 0) {
        sub_080483AC(p, g[10]);
        *(u32 *)(p + 0x4c) &= ~1;
    } else
        *(u32 *)(p + 0x4c) |= 1;
}
