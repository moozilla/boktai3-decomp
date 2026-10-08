#include "global.h"

void sub_080A2F04(u8 *);
void sub_0824923C(u8 *, u32);
void sub_0807FC5C(u8 *);

u32 sub_080A4A60(u8 *p) {
    u32 i;
    u32 *t;
    sub_080A2F04(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    sub_0824923C(p, t[i]);
    sub_0807FC5C(p);
    return 1;
}
