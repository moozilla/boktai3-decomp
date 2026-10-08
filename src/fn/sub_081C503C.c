#include "global.h"

struct S { u8 pad[8]; u32 f8; };

s32 sub_081C503C(struct S *p, s32 a, s32 b)
{
    s32 i;
    u32 fl;
    if (a > ((b + 1) * b) >> 1) return 1;
    i = 0;
    fl = p->f8;
    if (i < b) {
        a -= b;
        if (a > 0) {
            do {
                i++;
                if (i >= b) break;
                a -= b - i;
            } while (a > 0);
        }
    }
    if ((i & 1) == 0) fl |= 1;
    else fl &= ~1;
    p->f8 = fl;
    return 0;
}
