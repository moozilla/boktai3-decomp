#include "global.h"

u32 sub_080A0C04(u8 *);
void sub_0824923C(u8 *, u32);
void sub_0807FC5C(u8 *);
void sub_080A0C10(u8 *);

u32 sub_080A1358(u8 *p) {
    u32 i;
    u32 *t;
    sub_080A0C04(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    sub_0824923C(p, t[i]);
    sub_0807FC5C(p);
    sub_080A0C10(p);
    return 1;
}
