#include "global.h"
struct H { u8 p[4]; s16 w; s16 h; };
struct A { u8 p[4]; struct H *f4; };
extern struct A *gUnk_030052F4;
extern s32 gUnk_030052F8, gUnk_030052FC;
void sub_0821BBF8(void)
{
    gUnk_030052F8 = gUnk_030052F4->f4->w;
    gUnk_030052FC = gUnk_030052F4->f4->h;
}
