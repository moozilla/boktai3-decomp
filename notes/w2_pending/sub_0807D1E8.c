#include "global.h"

u32 sub_0822B694(u8 *, s8);
void sub_0807CDC4(u32, u32, u8 *, u8 *);

void sub_0807D1E8(u8 *p, u32 a, u32 b, s8 c) {
    u8 *q = p + 0xc;
    u32 r = sub_0822B694(q, c);
    *(u32 *)(p + 0x18) = r;
    if (r == 0xB546) sub_0807CDC4(a, b, q, q);
}
