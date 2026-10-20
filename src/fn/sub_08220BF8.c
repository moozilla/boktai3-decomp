#include "global.h"
/* Box_Overlaps: Tests whether two boxes overlap on all three axes (sum of half extents against center distance) */
#define Box_Overlaps sub_08220BF8

struct Box {
    u8 pad0[0x1c];
    s16 ext[3];
    s16 pad;
    s16 pos[3];
};

static inline s32 axis(struct Box *a, struct Box *b, s32 i)
{
    s32 x = a->ext[i];
    s32 y = b->ext[i];
    s16 e = x + y;
    s32 hit = 0;
    s32 pa = a->pos[i];
    if (pa - e <= b->pos[i] && b->pos[i] - e <= pa)
        hit = 1;
    return hit;
}

s32 sub_08220BF8(struct Box *a, struct Box *b)
{
    s32 result = 0;
    if (axis(a, b, 0) && axis(a, b, 1) && axis(a, b, 2))
        result = 1;
    return result;
}
