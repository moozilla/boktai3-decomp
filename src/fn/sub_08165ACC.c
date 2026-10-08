#include "global.h"
struct Q { u32 a; u32 b; u32 fl; };
void sub_08165ACC(u8 *p)
{
    u32 one = 1;
    s32 i;
    u32 *q = (u32 *)(p + 0x1428);
    struct Q *r;
    for (i = 0x7b; i >= 0; i--) {
        *q |= one;
        q += 0x18;
    }
    *(u32 *)(p + 0xC28) |= 1;
    r = (struct Q *)(p + 0x4304);
    r->fl |= 1;
    r = (struct Q *)(p + 0x42A4);
    r->fl |= 1;
    *(u32 *)(p + 0x184) |= 1;
}
