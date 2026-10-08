#include "global.h"
struct E { u32 fl; u8 f[0x50 - 4]; u16 h; u8 g[0x5c - 0x52]; };
struct G { u8 f[0x58]; u8 n; u8 g[3]; struct E e[32]; };
extern struct G *gUnk_02000240;
struct E *sub_081A4020(void)
{
    struct G *g = gUnk_02000240;
    struct E *e;
    s32 i;
    if (g->n <= 0x1f) goto setup;
    goto none;
found:
    return e;
setup:
    e = g->e;
    for (i = 0; i <= 0x1f; e++, i++) {
        if (e->h == 0 && (e->fl & 1) != 0) goto found;
    }
none:
    return 0;
}
