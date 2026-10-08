#include "global.h"

struct E { u16 used; u16 id; u32 ptr; };
struct S { u8 f[0x20]; s32 n; u8 g[0x6c]; struct E e[1]; };
u32 sub_08225984(u16);

void sub_08003B44(struct S *s, u16 id)
{
    struct E *e = s->e;
    s32 i = 0;
    while (i < s->n) {
        if (e->used == 0)
            break;
        e++;
        i++;
    }
    if (i != s->n) {
        e->ptr = sub_08225984(id);
        if (e->ptr != 0) {
            e->used = 1;
            e->id = id;
        }
    }
}
