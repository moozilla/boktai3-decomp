#include "debris_particles.h"

static inline void add(struct SceneVec *d, struct SceneVec *v)
{
    d->x += v->x;
    d->y += v->y;
    d->z += v->z;
}

int Debris_Update(void *unused, struct DebrisParticle *p)
{
    struct SceneVec *v = &p->velocity;
    SpritePool_AdvanceAnimation(p);
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

/* Spawns one debris piece at a barrier: random angle/speed, sprite pool resource 0xDA6D, update
 * callback Debris_Update. Blocked (about 69% fuzzy): the original keeps the particle (r8),
 * velocity pointer (r7), index pointer (sb) in high registers and loads a stray literal
 * 0x0403B714 that I could not reproduce. */
INCLUDE_ASM("asm/nonmatching", sub_08012B34);
