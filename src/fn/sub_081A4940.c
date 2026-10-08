#include "global.h"
struct E { u8 f[8]; u32 fl; u8 g[0xdc - 0xc]; u32 v; u8 h[0xe8 - 0xe0]; };
struct G { u8 f[0x62]; u16 n; struct E e[16]; };
extern struct G *gUnk_02000244;
struct E *sub_081A4940(void)
{
    struct G *g = gUnk_02000244;
    struct E *e;
    s32 i;
    if (g->n <= 0xf) goto setup;
    goto none;
found:
    return e;
setup:
    e = g->e;
    for (i = 0; i <= 0xf; e++, i++) {
        if (e->v == 0 && (e->fl & 1) != 0) goto found;
    }
none:
    return 0;
}
