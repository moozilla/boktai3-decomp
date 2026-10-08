#include "global.h"

struct P { u32 a; u32 b; };
void sub_082258AC(u8 *);

u32 sub_08225AB4(u8 *p, u32 a, u32 *q, u8 b, u8 c, u32 d) {
    u32 z;
    *(u32 *)p = a;
    z = 0;
    *(u32 *)(p + 4) = z;
    p[8] = c;
    *(struct P *)(p + 0xc) = *(struct P *)q;
    p[9] = b;
    *(u16 *)(p + 0x14) = z;
    *(u16 *)(p + 0x16) = z;
    *(u16 *)(p + 0x18) = z;
    *(u16 *)(p + 0x1a) = 0x10;
    *(u32 *)(p + 0x1c) = z;
    *(u32 *)(p + 0x24) = z;
    *(u32 *)(p + 0x2c) = z;
    *(u32 *)(p + 0x28) = z;
    sub_082258AC(p);
    *(u32 *)(p + 0x3c) = d;
    return 1;
}
