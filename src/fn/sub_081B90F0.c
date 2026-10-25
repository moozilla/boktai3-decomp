#include "orbit_motion.h"

u16 Sqrt(u32);
s32 Div(s32, s32);
extern const s16 gUnk_086149C4[];

static inline void difference(struct OrbitVector *out, struct OrbitVector *a, struct OrbitVector *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
static inline s32 square(s32 n)
{
    return n * n;
}
static inline void normalize(struct OrbitVector *v, s32 scale)
{
    s32 sum = square(v->x) + square(v->y);
    s32 length = Sqrt(sum + square(v->z));
    if (length) {
        v->x = Div(v->x * scale, length);
        v->y = Div(v->y * scale, length);
        v->z = Div(v->z * scale, length);
    }
}

s32 OrbitMotion_CalculateArcCenter(struct OrbitVector *out, struct OrbitVector *start,
                                   struct OrbitVector *end, s32 sweep)
{
    struct OrbitVector delta;
    s32 distanceSquared;
    s32 direction;
    s32 angle;
    s32 radius;
    if (!out || !start || !end)
        return 0;
    if (sweep < 0) {
        direction = -1;
        sweep = -sweep;
    } else {
        direction = 1;
    }
    sweep %= 256;
    difference(&delta, end, start);
    distanceSquared = square(delta.x) + square(delta.z);
    if (sweep == 0) {
        *out = *start;
        return 0;
    }
    if (sweep == 128) {
        add(out, start, end);
        out->x /= 2;
        out->z /= 2;
        out->y /= 2;
        return Sqrt(distanceSquared) / 2;
    }
    if (sweep < 128)
        angle = (128 - sweep) >> 1;
    else
        angle = (sweep - 128) >> 1;
    if (sweep > 128)
        direction = -direction;
    if (direction < 0)
        angle = (256 - angle) & 255;
    out->x =
        scale12(gUnk_086149C4[(angle + 64) & 255] * delta.x - gUnk_086149C4[angle & 255] * delta.z);
    out->z =
        scale12(gUnk_086149C4[angle & 255] * delta.x + gUnk_086149C4[(angle + 64) & 255] * delta.z);
    out->y = 0;
    radius = Sqrt(distanceSquared) / 2;
    radius = (radius << 12) / gUnk_086149C4[(angle + 64) & 255];
    normalize(out, radius);
    add(out, start, out);
    return radius;
}
