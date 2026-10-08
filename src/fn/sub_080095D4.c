#include "global.h"
struct V { u16 x, y, z; };
struct S { u8 f[2]; u8 t; u8 g[0x19]; struct V v; };
struct Q { u8 f[0x1c]; struct V v; };

u32 sub_080095D4(u32 unused, u8 *p, struct V *in)
{
    struct S *s = (struct S *)p;
    struct Q *q = (struct Q *)(p + 0x24);
    s->v.x = in->x;
    s->v.y = in->y;
    s->v.z = in->z;
    q->v.x = in->x;
    q->v.y = in->y;
    q->v.z = in->z;
    if (s->t != 0) {
        s->v.x += 0x80;
        q->v.z -= 0x80;
    } else {
        s->v.z += 0x80;
        q->v.x -= 0x80;
    }
    q->v.x += 0x3c;
    q->v.z += 0x3c;
    q->v.y += 0x14;
}
