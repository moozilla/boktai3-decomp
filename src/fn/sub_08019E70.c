#include "global.h"
void sub_0821980C(u8 *, u8 *, u32, u32);
void sub_08019E70(u8 *p, u8 *q, u8 *r)
{
    sub_0821980C(q, p + 0x6c, *r + 0xf0, 0);
    *(u32 *)(q + 8) &= ~1;
}
