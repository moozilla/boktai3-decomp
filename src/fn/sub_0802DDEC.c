#include "global.h"

struct Vec0802DDEC {
    s16 x, y, z;
};

static inline s32 Abs(s32 v)
{
    if (v < 0)
        v = -v;
    return v;
}

u32 sub_0802DDEC(struct Vec0802DDEC *a, struct Vec0802DDEC *b, s32 range)
{
    if (Abs(b->x - a->x) < range && Abs(b->y - a->y) <= 255 && Abs(b->z - a->z) < range)
        return 1;
    return 0;
}
