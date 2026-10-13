#include "global.h"

void sub_0821980C(u8 *, u8 *, u32, u32);

void sub_08040ABC(u8 *p)
{
    u8 *q = p + 0x2fc;
    sub_0821980C(q, p + 0x15c, 1, 0);
    *(u32 *)(q + 8) &= ~1;
}
