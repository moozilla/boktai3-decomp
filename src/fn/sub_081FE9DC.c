#include "global.h"
struct G { u8 p[0xeec]; s32 w; };
extern struct G *gUnk_020005EC;
void sub_081FE6E0(void);
s32 sub_081FE9DC(void)
{
    struct G *g = gUnk_020005EC;
    s32 r;
    if (g != 0)
        r = g->w;
    else {
        sub_081FE6E0();
        r = -1;
    }
    return r;
}
