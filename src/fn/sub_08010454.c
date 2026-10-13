#include "group_objects.h"
// MATCHING_CONCESSION: register-pinning
static inline void addPosition(struct GroupVec *d, struct GroupVec *s)
{
    struct GroupVec *dst = d;
    struct GroupVec *src = s;
    {
        u32 value = (u16)s->x;
        {
            register u32 other asm("r3") = (u16)d->x;
            value = value + other;
        }
        d->x = value;
    }
    dst->y += src->y;
    dst->z += src->z;
}
void BoardPiece_Launch(struct GroupObjects *m, struct GroupPiece *p, int delay)
{
    struct GroupVec delta;
    if (p->active)
        p->active = 0;
    polar(&delta, p->direction, p->duration * 2);
    addPosition(&p->position, &delta);
    sub_0800FA5C(m, p, 0);
    if (p->timer >= delay + 2) {
        p->state = 1;
        p->active = 1;
        p->timer = 0;
    }
    p->timer++;
}
