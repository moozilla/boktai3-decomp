#include "global.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
struct Position {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad[0x1C];
    struct Position position;
};
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void CallbackPool_SpawnHitParticles(void *);
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline u16 Rand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)((u32)gUnk_0203B400 + index);
}
static inline void Polar(s32 angle, struct Position *v, s32 radius)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
// confidence=low: removal caller emits five directional debris particles.
void ActorGroup_EmitRemovalDebris(struct Actor *p)
{
    struct Position v;
    {
        s32 angle = Rand() & 255;
        struct Position *vec = &v;
        asm("" : : "r"(angle), "r"(vec));
        Polar(angle, vec, 10);
    }
    v.y = 64;
    sub_08127590(&p->position, &v, 0x9385, 0x74C9, 1, 30, 2, 0, 490, 0, 0, 0, 0, 0, 0);
    Polar(Rand() & 255, &v, 10);
    v.y = 34;
    sub_08127590(&p->position, &v, 0x9385, 0x74C9, 1, 30, 2, 0, 490, 0, 0, 0, 0, 0, 0);
    Polar(Rand() & 255, &v, 10);
    v.y = 34;
    sub_08127590(&p->position, &v, 0x9385, 0x74C9, 1, 30, 2, 0, 490, 0, 1, 0, 0, 0, 0);
    Polar(Rand() & 255, &v, 10);
    v.y = 34;
    sub_08127590(&p->position, &v, 0x9385, 0x74C9, 1, 30, 2, 0, 490, 1, 0, 0, 0, 0, 0);
    Polar(Rand() & 255, &v, 10);
    v.y = 34;
    sub_08127590(&p->position, &v, 0x9385, 0x74C9, 1, 30, 2, 0, 490, 2, 0, 0, 0, 0, 0);
    CallbackPool_SpawnHitParticles(p);
}
