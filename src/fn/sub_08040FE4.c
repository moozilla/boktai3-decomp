#include "global.h"

void sub_08040FE4(u8 *p)
{
    u32 m = 1;
    u32 *q = (u32 *)(p + 0x184);
    s32 i = 0x11;
    do {
        *q |= m;
        q = (u32 *)((u8 *)q + 0x60);
        i--;
    } while (i >= 0);
    m = 1;
    q = (u32 *)(p + 0xE3C);
    i = 4;
    do {
        *q |= m;
        q = (u32 *)((u8 *)q + 0x60);
        i--;
    } while (i >= 0);
}
