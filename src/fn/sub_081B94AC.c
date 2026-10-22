#include "orbit_motion.h"
u16 Sqrt(u32);
s32 __divsi3(s32, s32);
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
static inline s32 distance(struct OrbitVector *v)
{
    return Sqrt(square(v->x) + square(v->y));
}
void sub_081B94AC(struct OrbitMotion *p, struct OrbitVector *position, struct OrbitVector *end,
                  s32 angle, s32 speed)
{
    struct OrbitVector delta;
    if (p && position && end) {
        struct OrbitVector *target;
        p->position = position;
        target = &p->base;
        *target = *end;
        p->angle = angle;
        difference(&delta, target, p->position);
        {
            s32 length = distance(&delta);
            s32 factor = 314 * speed;
            p->radius = __divsi3(length * factor, 25600);
        }
        if (p->radius <= 1)
            p->radius = 2;
        p->speed = speed;
        p->unk28 = 7;
        p->state = 1;
        p->timer = 0;
    }
}

void sub_081B9550(struct OrbitMotion *p, struct OrbitVector *position, s32 radius, s32 radius2,
                  s32 duration)
{
    if (p && position && duration) {
        p->position = position;
        p->base = *position;
        p->radius = radius;
        p->radius2 = radius2;
        p->speed = __divsi3(0x200, duration);
        p->unk28 = 9;
        p->state = 1;
        p->timer = 0;
    }
}
