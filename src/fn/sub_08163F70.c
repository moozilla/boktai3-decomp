#include "global.h"

struct E { u8 f00[0x2c]; u16 *w; };
extern struct E gUnk_03004C30[];

u16 *sub_08163F70(s32 i, u32 x, u32 y)
{
    struct E *e;
    u16 *p;
    u32 m;
    e = &gUnk_03004C30[i];
    p = e->w;
    m = 0x1f;
    p += x & m;
    p = (u16 *)((u8 *)p + ((y & m) << 6));
    return p;
}
