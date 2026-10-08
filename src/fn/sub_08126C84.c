#include "global.h"
extern u32 gUnk_0200015C;
void sub_08214514(u8 *);
u32 sub_08126C84(u8 *p)
{
    u8 *q = p + 0x38;
    u8 *r = p + 0x77;
    s32 n = 0x7f;
    u32 z;
    do {
        if (*r == 1 && q[4] != 0)
            sub_08214514(q);
        q += 0x40;
        r += 0x40;
        n--;
    } while (n >= 0);
    z = 0; gUnk_0200015C = z; return z;
}
