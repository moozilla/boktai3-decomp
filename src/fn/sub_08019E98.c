#include "global.h"
u8 *sub_0822D6C8(u32);
void sub_08019E70(u8 *, u8 *, u8 *);
void sub_08019E98(u8 *p, u8 *q, u32 c)
{
    u8 *r = sub_0822D6C8(c);
    if (*r != 0xff) {
        sub_08019E70(p, q, r);
        *(u32 *)(q + 8) &= ~1;
    } else {
        *(u32 *)(q + 8) |= 1;
    }
}
