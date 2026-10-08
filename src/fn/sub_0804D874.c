#include "global.h"

void sub_0804D874(u8 *p)
{
    u32 m = 1;
    s32 i;
    u32 *q = (u32 *)(p + 0xa0);
    for (i = 3; i >= 0; i--) {
        *q |= m;
        q = (u32 *)((u8 *)q + 0x2c);
    }
}
