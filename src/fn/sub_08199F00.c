#include "global.h"

void sub_082333A8(u8 *, u32, u32, u32, u32);
void sub_0824923C(u8 *, u32);

void sub_08199F00(u8 *p, u32 unused, u8 *q) {
    u8 *c = p + 0xC;
    u16 a2 = *(u16 *)(p + 0x3C);
    u16 a3 = *(u16 *)(p + 0x40);
    u32 z = 0;
    u32 w;
    sub_082333A8(c, 1, a2, a3, z);
    *(u32 *)q = z;
    w = *(u32 *)(q + 4);
    if (w) {
        sub_0824923C(q, w);
        *(u32 *)(q + 4) = z;
    }
}
