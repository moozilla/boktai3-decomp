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
extern u32 gUnk_03005308;
static inline u16 Debris_NextRandom(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return ((const u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308];
}
static inline int Debris_Scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
#ifdef NONMATCHING
// fuzzy: 53.5%
// scope shape 75 (differences once low registers are set aside)
s32 sub_08012B34(void *arg0, struct BreakableBarrier *b)
{
    struct DebrisParticle *p = SpritePool_Allocate();
    struct SceneVec *v;
    struct SceneVec pos;
    s32 angle, speed;
    if (p == NULL)
        return -1;
    v = &p->velocity;
    if (b->sprite.flags & 4)
        angle = (Debris_NextRandom() & 0x7F) + 0x40;
    else
        angle = (Debris_NextRandom() & 0x7F) - 0x40;
    speed = (Debris_NextRandom() & 7) + 8;
    v->x = Debris_Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = Debris_Scale(gUnk_086149C4[angle & 255] * speed);
    v->y = (Debris_NextRandom() & 31) + 16;
    pos = b->sprite.position;
    pos.x = pos.x - 32 + (Debris_NextRandom() & 63);
    pos.y = pos.y - 32 + (Debris_NextRandom() & 63);
    pos.z = pos.z - 32 + (Debris_NextRandom() & 63);
    SpritePool_SetResource(p, 0xDA6D);
    SpritePool_StartAnimation(p, 0x5BB7, 1, 0, Debris_NextRandom() & 1);
    SpritePool_SetPosition(p, &pos);
    SpritePool_Activate(p, Debris_Update, b);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08012B34);
#endif
