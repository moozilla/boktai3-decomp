#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u32 flags;
    u8 pad[20];
    struct Vec position;
    u8 tail[8];
};
struct Orb {
    u8 state, angle;
    u16 rise, radius, timer;
    struct Sprite sprite;
};
struct Manager {
    u8 pad[24];
    int time, duration;
    u16 speed, limit, count, frame;
    void *resource;
    struct Vec position;
    struct Orb orbs[8];
};
extern const s16 gUnk_086149C4[];
void sub_080187E4(void *, void *);
void OrbitingParticles_Advance(void *, void *);
int sub_080187BC(void *);
void sub_0821A0C0(void *);
static inline int scale(int v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
int OrbitingParticles_Update(struct Manager *m)
{
    int i;
    i = 0;
    if (i < m->count) {
        struct Vec *position = &m->orbs[0].sprite.position;
        struct Orb *cursor = m->orbs;
        struct Orb *p = cursor;
        do {
            int angle, radius;
            struct Vec *out;
            register const s16 *table asm("r2");
            struct Orb *arg = cursor;
            asm("" : "+r"(arg));
            if (!p->state)
                sub_080187E4(m, arg);
            else
                OrbitingParticles_Advance(m, arg);
            out = position;
            angle = p->angle;
            radius = p->radius;
            table = gUnk_086149C4;
            out->x = scale(table[(angle + 64) & 255] * radius);
            out->y = 0;
            out->z = scale(table[angle] * radius);
            p->sprite.position.y += p->rise;
            p->sprite.position.x += m->position.x;
            p->sprite.position.y += m->position.y;
            p->sprite.position.z += m->position.z;
            position = (void *)((u8 *)position + 48);
            p++;
            cursor++;
            i++;
        } while (i < m->count);
    }
    {
        u32 *flags;
        i = m->count;
        if (i <= 7) {
            int mask = 1, offset;
            offset = i * 48 + 60;
            flags = (void *)(offset + (u32)m);
            do {
                *flags |= mask;
                flags = (void *)((u8 *)flags + 48);
                i++;
            } while (i <= 7);
        }
    }
    m->time++;
    if (m->duration > 0 && m->time >= m->duration && !sub_080187BC(m))
        sub_0821A0C0(m);
    return 0;
}
