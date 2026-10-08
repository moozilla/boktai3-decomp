#include "global.h"
struct S { u8 f[0xe]; u16 h; };
u32 sub_081CEDD4(struct S *p, s32 b, s32 c)
{
    s32 n;
    if (b > (c * (c + 1)) >> 1) return 1;
    n = 0;
    if (n < c) {
        b -= c;
        if (b > 0) {
            do {
                n++;
                if (n >= c) break;
                b -= c - n;
            } while (b > 0);
        }
    }
    if ((n & 1) == 0) {
        u32 m = 0x100;
        p->h = m | p->h;
    }
    return 0;
}
