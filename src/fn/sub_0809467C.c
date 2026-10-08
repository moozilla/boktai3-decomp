#include "global.h"

u32 sub_080937E8(u8 *);
void sub_0824923C(u8 *, u32);
void sub_0807FC5C(u8 *);
void sub_080937F4(u8 *);

u32 sub_0809467C(u8 *p) {
    u32 i;
    u32 *t;
    sub_080937E8(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    sub_0824923C(p, t[i]);
    sub_0807FC5C(p);
    sub_080937F4(p);
    return 1;
}
