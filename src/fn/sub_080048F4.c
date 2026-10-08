#include "global.h"

struct E { u8 f[0x1e]; u8 on; u8 g[0xd]; u32 k; u8 h[0x178]; };
struct S { u8 f[0x18]; u16 n; u8 g[0x56]; struct E *e; };
extern struct S *gUnk_02000024;
u32 sub_0821ABA8(u32, u32);
void sub_080047AC(void *);
s32 sub_080048F4(void)
{
    struct S *s = gUnk_02000024;
    u32 k;
    struct E *e;
    s32 i;
    if (s == 0)
        return -1;
    k = sub_0821ABA8(0x6e, 0);
    e = s->e;
    for (i = 0; i < s->n; e++, i++) {
        if (e->on != 0 && e->k == k)
            sub_080047AC(e);
    }
    return 0;
}
