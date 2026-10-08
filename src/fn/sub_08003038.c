#include "global.h"

void sub_08002C38(u8 *a, u8 *b);

void sub_08003038(u8 *p) {
    s32 i = 3;
    u32 v = (u32)p + 0x88C;
    u32 *t = (u32 *)(p + 0x288);
    u32 step = -0x200;
    do {
        *t = v;
        v += step;
        t--;
        i--;
    } while (i >= 0);
    sub_08002C38(p, p + 0x4C);
}
