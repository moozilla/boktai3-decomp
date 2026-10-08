#include "global.h"
struct E { u16 a; u16 b; u32 c; };
extern struct E gUnk_03005260[];
u32 sub_08221338(u32 idx, u8 *p, u32 m, u32 c, s32 max)
{
    u32 ret = 0;
    s32 v = *p;
    struct E *t = gUnk_03005260;
    struct E *e = t + idx;
    if (e->b & m) v = max;
    else if (e->a & m) {
        if (v >= max) goto wrap;
        v++;
    } else v = 0;
    if (v >= max) {
wrap:
        v = 0;
        ret = c;
    }
    *p = v;
    return ret;
}
