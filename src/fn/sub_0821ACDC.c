#include "global.h"
struct T { u32 *tbl; u32 f4; u8 *base; };
extern struct T gUnk_02000438;
u8 *sub_0821ACDC(u32 id, u32 *out)
{
    u32 idx = (id & 0x7FFFFFFF) - 1;
    struct T *t = &gUnk_02000438;
    u32 *e = t->tbl + idx;
    *out = ((u8 *)e)[3];
    return t->base + (e[0] & 0xFFFFFF);
}
