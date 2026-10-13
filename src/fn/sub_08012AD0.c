#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Particle {
    u8 pad[8];
    u32 timer, flags;
    u8 pad10[24];
    struct Vec position;
    u8 pad30[60];
    struct Vec velocity;
};
void sub_08019C9C(void *);
static inline void add(struct Vec *d, struct Vec *v)
{
    d->x += v->x;
    d->y += v->y;
    d->z += v->z;
}
#define Debris_Update sub_08012AD0
int Debris_Update(void *unused, struct Particle *p)
{
    struct Vec *v = &p->velocity;
    sub_08019C9C(p);
    v->y -= 3;
    add(&p->position, v);
    if (p->timer > 14) {
        if ((p->timer & 3) <= 1)
            p->flags |= 1;
        else
            p->flags &= ~1;
    }
    if (p->timer > 29)
        return 1;
    return 0;
}
