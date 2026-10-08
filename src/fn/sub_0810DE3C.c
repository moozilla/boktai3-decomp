#include "global.h"

struct E { u32 f28; u8 f[0x5c]; };
void sub_0811CD24(u8 *, s32);

void sub_0810DE3C(u8 *p)
{
    u32 m = 1;
    u32 *q = (u32 *)(p + 0x28);
    s32 i = 9;
    do {
        *q |= m;
        q = (u32 *)((u8 *)q + 0x60);
        i--;
    } while (i >= 0);
    sub_0811CD24(p + 0xa8c, 1);
}
