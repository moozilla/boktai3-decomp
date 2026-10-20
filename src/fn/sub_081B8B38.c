#include "global.h"

#ifdef NONMATCHING
// fuzzy: 94%
// remaining diffs: load order of the target pointer, Sqrt result zero-extend, one pointer copy

struct MoverVec {
    s16 x, y, z, pad;
};
struct SceneMover {
    struct MoverVec *target;
    u8 pad4[8];
    struct MoverVec position;
    struct MoverVec velocity;
    u8 pad1C[0x29 - 0x1c];
    u8 state;
    u8 heading;
    u8 turnRate;
    s16 speed;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
u16 Sqrt(u32);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
s32 SceneMover_SteerXY(struct SceneMover *m)
{
    s32 result = 0;
    struct MoverVec *position = &m->position;
    s32 angle = Math_Atan2(m->position.x - m->target->x, position->y - m->target->y);
    s32 x = m->target->x - m->position.x;
    s32 z = m->target->y - position->y;
    s32 d2 = x * x + z * z;
    s32 next;
    s32 delta;
    s32 old;
    if (d2 <= m->speed * m->speed) {
        struct MoverVec *v;
        u32 dist = Sqrt(d2);
        v = (struct MoverVec *)((u8 *)m + 0x14);
        v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * dist);
        v->y = Scale(gUnk_086149C4[angle & 255] * dist);
        v->z = 0;
        m->state = 2;
        result = 1;
    }
    {
        struct MoverVec *v, *t;
        old = m->heading;
        delta = (angle - old) & 255;
        if (delta > 128 && delta < 256 - m->turnRate)
            next = old - m->turnRate;
        else if (delta <= 128 && delta > m->turnRate)
            next = m->heading + m->turnRate;
        else
            next = angle;
        m->heading = next;
        v = &m->velocity;
        {
            s32 speed = m->speed;
            v->x = Scale(gUnk_086149C4[(next + 64) & 255] * speed);
            v->y = Scale(gUnk_086149C4[next & 255] * speed);
            v->z = 0;
        }
        t = m->target;
        t->x += m->velocity.x;
        t->y += v->y;
        t->z += v->z;
    }
    return result;
}
#else
INCLUDE_ASM("asm/nonmatching", SceneMover_SteerXY);
#endif
