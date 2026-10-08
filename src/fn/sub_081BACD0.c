#include "global.h"

struct El { u16 a; u16 b; u8 pad[12]; };
struct Q { u8 f[0x98]; u16 x; u16 y; u8 gx[4]; s16 idx; u8 g[2]; u8 g3[0]; struct El e[1]; };

void sub_081BACD0(struct Q *p)
{
    if (p->idx >= 0) {
        p->x = p->e[p->idx].a;
        p->y = p->e[p->idx].b - 10;
    }
}
