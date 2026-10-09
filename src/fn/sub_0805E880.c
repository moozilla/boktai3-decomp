#include "global.h"
u8 sub_0805E5D0(void *);
void sub_0822B2F8(u32);
struct S { u8 f0; u8 b1; u8 b2; u8 b3; u8 f4[8]; s8 t[0]; };
u32 sub_0805E880(struct S *p) {
    s32 base = p->t[p->b2 + p->b1 * 4];
    u32 i = (p->b2 + 1) & 3;
    while (i != p->b2) {
        s32 v = p->t[i + p->b1 * 4];
        if (v >= 0 && v != base) {
            p->b2 = i;
            p->b3 = sub_0805E5D0(p);
            sub_0822B2F8(0xdc);
            return 1;
        }
        i = (i + 1) & 3;
    }
    return 0;
}
