#include "global.h"
#include "ram.h"
struct E {
    u32 fl;
    u8 f[0x8a - 4];
    u16 h;
    u8 g[0x90 - 0x8c];
};
struct G {
    u8 f[0x3c];
    u32 n;
    u8 g[8];
    struct E e[32];
};
struct E *Hresvelgr_FindInactiveShot(void)
{
    struct G *g = gDirectionBurstEmitter;
    struct E *e;
    s32 i;
    if (g->n <= 0x1f)
        goto setup;
    goto none;
found:
    return e;
setup:
    e = g->e;
    for (i = 0; i <= 0x1f; e++, i++) {
        if ((e->fl & 1) != 0 && e->h == 0)
            goto found;
    }
none:
    return 0;
}
