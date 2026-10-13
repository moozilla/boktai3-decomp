#include "global.h"

struct S {
    u8 f[0xd0];
    u32 a;
};
extern u32 gUnk_020000FC;
void sub_08033778(u32);
void Bg_ClearTilemap(u32);
u32 sub_080390A0(struct S *p)
{
    sub_08033778(p->a);
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(2);
    return gUnk_020000FC = 0;
}
