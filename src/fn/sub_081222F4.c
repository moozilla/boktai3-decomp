#include "global.h"
struct S { u8 f[9]; u8 a; };
void sub_081222F4(struct S *s, u8 *p, u8 *q)
{
    u32 v = ((s->a + 0x60) >> 6) & 3;
    if (v > 1) {
        *q = 1;
        *p = 3 - v;
    } else {
        *q = 0;
        *p = v;
    }
}
