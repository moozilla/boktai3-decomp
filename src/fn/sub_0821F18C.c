#include "global.h"
struct R { u8 x0, y0, x1, y1; u8 p[12]; };
struct P { s16 x; s16 p; s16 y; };
u32 sub_0821F18C(struct R *r, struct P *pt, u32 i)
{
    struct R *e = r + i;
    s32 x = pt->x;
    if (x >= (e->x0 << 8) && x < (e->x1 << 8)) {
        s32 y = pt->y;
        if (y >= (e->y0 << 8) && y < (e->y1 << 8))
            return 1;
    }
    return 0;
}
