#include "global.h"

void sub_08076034(u8 *, u32);

static inline u8 Consume(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void sub_08082BF0(u8 *p)
{
    u8 *g = *(u8 **)(p + 0x3d0);
    u8 c = Consume(p + 0x2b2);
    if (c) {
        u8 v;
        sub_08076034(p, 1);
        v = p[0x59];
        g[0x3dd] = v;
    } else if (g[0x3dd] != p[0x59]) {
        sub_08076034(p, 1);
        g[0x3dd] = p[0x59];
    } else
        p[0x2b3] = c;
}
