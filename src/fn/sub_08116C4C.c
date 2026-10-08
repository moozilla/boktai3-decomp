#include "global.h"
struct Q { u8 f[0x28]; u32 a, b, c; };
extern struct Q *gUnk_020004F0[];
void sub_08116C4C(s32 i, u32 *d)
{
    struct Q *q;
    if (i <= 1) {
        q = gUnk_020004F0[i];
        if (q != 0) {
            d[0] = q->a;
            d[1] = q->b;
            d[2] = q->c;
        }
    }
}
