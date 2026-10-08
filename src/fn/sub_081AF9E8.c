#include "global.h"

extern s32 gUnk_0300523C;
struct A { u8 f[0x2c]; u8 g[0x86]; s16 h; };
void sub_08233140(void *, void *, s32, s32, s32, s32);

void sub_081AF9E8(struct A *p)
{
    if (gUnk_0300523C <= 0 && p->h != 0)
        sub_08233140(p, (u8 *)p + 0x2c, 8, 0x200, -1, 0);
}
