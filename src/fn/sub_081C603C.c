#include "global.h"
struct S { u8 f[0x60]; u32 fl; u8 g[0x370 - 0x64]; s32 cnt; u8 h[0x358 - 0x374 + 0x100]; };
struct T { u8 f[0x60]; u32 fl; u8 g[0x358 - 0x64]; s32 st; u8 h[0x370 - 0x35c]; s32 cnt; };
extern u16 gUnk_03005260[];
void sub_0822B2F8(u32);
void sub_081C5EF0(void *, void (*)(void *));
void sub_081C61A0(void *);
void sub_081C603C(struct T *p)
{
    p->cnt++;
    if (p->cnt > 0x3c || (gUnk_03005260[1] & 1) != 0) {
        sub_0822B2F8(0x167);
        if ((u32)(p->st - 1) <= 4) p->fl &= ~1;
        sub_081C5EF0(p, sub_081C61A0);
    }
}
