// MATCHING_CONCESSION: register-pinning
#include "debris_particles.h"
#include "engine.h"

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

/* Spawns debris at a barrier with a random angle, speed and position. */
static inline u16 Debris_EarlyRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
}
static inline u16 Debris_NextRandom(void)
{
    const u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
static inline int Debris_Scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
// scope shape 75 (differences once low registers are set aside)
s32 sub_08012B34(void *arg0, struct BreakableBarrier *b)
{
    struct DebrisParticle *p = SpritePool_Allocate();
    struct SceneVec *v;
    struct SceneVec pos;
    struct SceneVec *position;
    s32 angle, speed;
    /* The original RNG expansion retains this unused setup constant. */
    register u32 setup asm("r3");
    if (p == NULL)
        return -1;
    v = &p->velocity;
    if (b->sprite.flags & 4) {
        setup = 0x0403B714;
        angle = (Debris_EarlyRandom() & 0x7F) + 0x40;
    } else {
        setup = 0x0403B714;
        angle = (Debris_EarlyRandom() & 0x7F) - 0x40;
    }
    speed = (Debris_EarlyRandom() & 7) + 8;
    v->x = Debris_Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = Debris_Scale(gUnk_086149C4[angle & 255] * speed);
    asm volatile("" ::"r"(setup));
    v->y = (Debris_NextRandom() & 31) + 16;
    position = &pos;
    pos = b->sprite.position;
    {
        s32 sample = Debris_NextRandom();
        s32 coordinate = (u16)position->x - 32;
        sample &= 63;
        position->x = coordinate + sample;
    }
    {
        s32 sample = Debris_NextRandom();
        s32 coordinate = (u16)position->y - 32;
        sample &= 63;
        position->y = coordinate + sample;
    }
    {
        s32 sample = Debris_NextRandom();
        s32 coordinate = (u16)position->z - 32;
        sample &= 63;
        position->z = coordinate + sample;
    }
    SpritePool_SetResource(p, 0xDA6D);
    SpritePool_StartAnimation(p, 0x5BB7, 1, 0, Debris_NextRandom() & 1);
    SpritePool_SetPosition(p, &pos);
    SpritePool_Activate(p, Debris_Update, b);
    return 0;
}
