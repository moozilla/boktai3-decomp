#include "global.h"

struct E { u16 id; u8 f[0x6a]; };
struct S { u8 f[0x18]; u8 n; u8 g[3]; u32 mask; struct E *e; };

struct E *sub_08009568(struct S *s, u32 id, s32 *out)
{
    struct E *e;
    s32 i;
    if (out != 0)
        *out = -1;
    e = s->e;
    for (i = 0; i < s->n; i++, e++) {
        if (((1 << i) & s->mask) && e->id == id) {
            if (out != 0)
                *out = i;
            return e;
        }
    }
    return 0;
}
