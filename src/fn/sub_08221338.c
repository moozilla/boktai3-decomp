#include "global.h"
#include "input.h"
struct E {
    u16 a;
    u16 b;
    u32 c;
};
u32 Input_UpdateKeyRepeat(u32 idx, u8 *p, u32 m, u32 c, s32 max)
{
    u32 ret = 0;
    s32 v = *p;
    struct E *t = gPadState;
    struct E *e = t + idx;
    if (e->b & m)
        v = max;
    else if (e->a & m) {
        if (v >= max)
            goto wrap;
        v++;
    } else
        v = 0;
    if (v >= max) {
    wrap:
        v = 0;
        ret = c;
    }
    *p = v;
    return ret;
}
