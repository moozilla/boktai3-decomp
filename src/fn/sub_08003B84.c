#include "global.h"

struct E { u16 used; u16 id; u32 ptr; };
struct S { u8 f[0x20]; s32 n; u8 g[0x6c]; struct E e[1]; };

void sub_08003B84(struct S *s, u16 id)
{
    struct E *e = s->e;
    s32 i;
    for (i = 0; i < s->n; e++, i++) {
        if (e->id == id) {
            e->used = 0;
            e->ptr = 0;
            e->id = 0;
            return;
        }
    }
}
