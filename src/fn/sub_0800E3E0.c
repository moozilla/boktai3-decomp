#include "global.h"

struct T { u8 f[0x18]; s16 w; u8 g[0x12]; u16 *p; };
extern struct T gUnk_03004C30[];

u16 *sub_0800E3E0(u32 i, u32 a, s32 b)
{
    struct T *t = &gUnk_03004C30[i];
    u16 *p = t->p;
    p = p + a;
    return p + (t->w << 1) * b;
}
