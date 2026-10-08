#include "global.h"

struct P { u16 x; u16 y; };
struct A { u8 filler[8]; struct P *p; };
struct B { s16 x; s16 pad; s16 y; };

u32 sub_08100F28(struct A *a, struct B *b, u32 c, u32 d)
{
    struct P *p = a->p;
    s32 px = p->x;
    s32 bx = b->x;
    s32 py = p->y;
    s32 by = b->y;
    s32 dy = py - by;
    s32 dx = px - bx;
    if (dx < 0)
        dx = -dx;
    if (dx <= c) {
        if (dy < 0)
            dy = -dy;
        if (dy <= d)
            return 1;
    }
    return 0;
}
