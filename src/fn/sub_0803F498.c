#include "global.h"
struct E { u8 d[0x130]; };
struct S { u8 f0[0x18]; u32 mask; struct E e[8]; };
u8 *sub_0803F498(struct S *p, s32 *out) {
    u8 *e = (u8 *)p + 0x20;
    s32 i = 0;
    u32 one = 1;
    u32 m = p->mask;
    do {
        if (((one << i) & m)) {
            i++;
            e += 0x130;
        } else {
            *out = i;
            return e;
        }
    } while (i <= 7);
    return 0;
}
