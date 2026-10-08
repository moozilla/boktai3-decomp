#include "global.h"

void sub_08249240(u8 *, u32, u32);

void sub_0807FC5C(u8 *p) {
    u16 *q;
    u32 v, f;
    p[0x2b0] = 0;
    q = (u16 *)(p + 0x2c8);
    v = (*q)++;
    f = *(u32 *)(p + 0x21c);
    if (f != 0) sub_08249240(p, v, f);
}
