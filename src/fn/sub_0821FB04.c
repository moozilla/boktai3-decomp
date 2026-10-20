#include "global.h"
extern const s16 gUnk_086149C4[];
s32 sub_0821BF5C(s16 *, s16 *, s32);
struct Vec3 {
    s16 x, y, z;
};
static inline s32 Mul12(s32 a, s32 b)
{
    s32 t = a * b;
    if (t >= 0)
        return t >> 12;
    return -(-t >> 12);
}
#define Actor_MoveAlongAngle sub_0821FB04
s32 Actor_MoveAlongAngle(struct Vec3 *pos, u32 angle, s32 dist)
{
    struct Vec3 d;
    struct Vec3 *p = &d;
    struct Vec3 *q;
    p->x = Mul12(dist, gUnk_086149C4[(angle + 64) & 255]);
    p->y = 0;
    p->z = Mul12(dist, gUnk_086149C4[angle & 255]);
    q = &d;
    d.x = d.x + pos->x;
    q->y = pos->y + q->y;
    q->z = q->z + pos->z;
    return sub_0821BF5C((s16 *)pos, (s16 *)&d, 4);
}
