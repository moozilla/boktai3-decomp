#include "global.h"
struct Vec2 {
    s32 x, y;
};
static inline s32 T12(s32 v)
{
    s32 r;
    if (v >= 0)
        r = v >> 12;
    else
        r = -(-v >> 12);
    return r;
}
s32 sub_081CFCF8(struct Vec2 *a, struct Vec2 *b)
{
    s32 dx = T12(b->x - a->x);
    s32 dy = T12(b->y - a->y);
    return dx * dx + dy * dy;
}