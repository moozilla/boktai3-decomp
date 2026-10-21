#include "global.h"

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
s32 SceneMover_SteerXZ(struct SceneMover *m)
{
    s32 result = 0;
    struct MoverVec *tg = m->target;
    struct MoverVec *position = &m->position;
    s32 angle;
    s32 x;
    s32 z;
    s32 d2;
    s32 next;
    s32 delta;
    s32 old;
    {
        s32 ax = m->position.x - tg->x;
        s32 ay = position->z - tg->z;
        angle = Math_Atan2(ax, ay);
    }
    {
        struct MoverVec *tg2 = m->target;
        x = tg2->x - m->position.x;
        z = tg2->z - position->z;
    }
    d2 = x * x + z * z;
    if (d2 <= m->speed * m->speed) {
        struct MoverVec *v;
        u16 d16 = Sqrt(d2);
        u32 dist;
        v = (struct MoverVec *)((u8 *)m + 0x14);
        dist = d16;
        v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * dist);
        v->y = 0;
        v->z = Scale(gUnk_086149C4[angle & 255] * dist);
        m->state = 2;
        result = 1;
    }
    {
        struct MoverVec *v, *t, *w;
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
            s32 prod = gUnk_086149C4[(next + 64) & 255] * speed;
            w = v;
            v->x = Scale(prod);
            v->y = 0;
            v->z = Scale(gUnk_086149C4[next & 255] * speed);
        }
        t = m->target;
        t->x += m->velocity.x;
        t->y += w->y;
        t->z += w->z;
    }
    return result;
}
