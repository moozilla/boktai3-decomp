#include "global.h"
struct T { u8 p[4]; u8 (*tbl)[4]; u8 *base; };
extern struct T gUnk_02000448;
u8 *Text_LookupString(u32 i)
{
    struct T *t = &gUnk_02000448;
    u8 *e = (u8 *)((i << 2) + (u32)t->tbl);
    u32 v = (e[3] << 24) | (e[2] << 16) | (e[1] << 8) | e[0];
    return t->base + (v & 0x7FFFFFFF);
}
