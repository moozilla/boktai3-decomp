#include "global.h"

struct E { u8 f[0x1e]; u8 act; u8 g[0x0d]; u32 key; u8 h[0x1a8-0x30]; };
struct S { u8 f[0x18]; u16 n; u8 g[0x56]; struct E *e; };

struct E *sub_080057FC(struct S *s, u32 key)
{
    struct E *e = s->e;
    s32 i;
    for (i = 0; i < s->n; e++, i++) {
        if (e->act != 0 && e->key == key)
            return e;
    }
    return 0;
}
