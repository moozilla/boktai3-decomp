#include "global.h"
struct G { u8 p[0x1c]; u32 w1c; };
extern struct G *gUnk_020005E8;
u32 sub_081FC66C(void)
{
    struct G *g = gUnk_020005E8;
    u32 r;
    if (g->w1c == 0)
        r = 0;
    else {
        g->w1c = 0;
        r = 1;
    }
    return r;
}
