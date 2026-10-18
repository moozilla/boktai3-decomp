#include "global.h"
struct E {
    u32 a, b, c, d;
};
struct S {
    u8 f[0x18];
    struct E e[10];
    u8 g[0xb8 - 0x18 - 0xa0];
    s32 m;
    s32 avg;
};
s32 SceneTilemap_GetLayoutLength(void);
s32 Div(s32, s32);
void Collider_InitManagerBuckets(struct S *s)
{
    s32 i;
    s32 v;
    s->m = -1;
    i = 0;
    do {
        s->e[i].a = 0;
        s->e[i].b = 0;
        s->e[i].c = 0;
        s->e[i].d = 0;
        i++;
    } while (i <= 9);
    v = SceneTilemap_GetLayoutLength() << 5;
    if (v <= 0)
        v = 0x7fffffff;
    s->avg = Div(v, 9);
}
