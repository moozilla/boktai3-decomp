// MATCHING_CONCESSION: register-pinning
#include "scene_particle_burst.h"
// confidence=low: spawns pooled particles with optional velocity ranges and position jitter.
#define SceneParticleBurst_CreateWithRanges sub_08067E3C
#ifdef NONMATCHING
// fuzzy: 61.6%
// Remaining: packed velocity temporaries, loop and pointer/register lifetimes.
extern struct SceneParticleBurstPool *gUnk_02000130;
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
struct SceneParticleBurstPool *SceneParticleBurst_EnsureSingleton(u32, u32);
struct SceneParticleBurstSlot *SceneParticleBurst_AllocateSlot(struct SceneParticleBurstPool *);
s32 Mod(s32, s32);
void ParticleSprite_Initialize(void *, const void *, u32);
void Effect_SetOrigin(void *, s32, s32);
static inline u32 Random(const u16 *table)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return table[gUnk_03005308];
}
s32 SceneParticleBurst_CreateWithRanges(const struct ContactPosition *position,
                                        const struct ContactPosition *velocity0,
                                        const struct ContactPosition *jitter0, s32 count,
                                        u32 priority, u32 flip, u32 flags)
{
    register const struct ContactPosition *velocity asm("r8") = velocity0;
    register const struct ContactPosition *jitter asm("sb") = jitter0;
    struct SceneParticleBurstPool *pool;
    s32 i;
    register const u16 *table asm("sl");
    if (!position)
        return -1;
    pool = gUnk_02000130;
    if (!pool) {
        pool = SceneParticleBurst_EnsureSingleton(0, 0);
        if (!pool)
            return -1;
    }
    i = 0;
    table = gUnk_0203B400;
    for (; i < count; i++) {
        register struct SceneParticleBurstSlot *p asm("r6") = SceneParticleBurst_AllocateSlot(pool);
        struct SceneParticleEffect *effect;
        u32 sequence;
        if (!p)
            return -1;
        effect = &pool->effects[p->index];
        p->position = *position;
        if (jitter->x)
            p->position.x += Mod(Random(table), jitter->x * 2) - jitter->x;
        if (jitter->y)
            p->position.y += Mod(Random(table), jitter->y * 2) - jitter->y;
        if (jitter->z)
            p->position.z += Mod(Random(table), jitter->z * 2) - jitter->z;
        if (!velocity) {
            p->velocity.x = 0;
            p->velocity.y = 0;
            p->velocity.z = 0;
        } else {
            struct ContactPackedVector value;
            if (!velocity->x)
                value.x = 0;
            else
                value.x = Mod(Random(table), velocity->x);
            if (!velocity->y)
                value.y = 0;
            else
                value.y = Mod(Random(table), velocity->y);
            if (!velocity->z)
                value.z = 0;
            else
                value.z = Mod(Random(table), velocity->z);
            *(struct ContactPackedVector *)&p->velocity = value;
        }
        p->delay = Mod(Random(table), 16);
        ParticleSprite_Initialize(effect, pool->graphicsResource, 0);
        Effect_SetOrigin(effect, -8, -8);
        sequence = (*(u32 *)((u8 *)gUnk_02000710 + 0x868) & 8) ? 14 : 13;
        Anim_SetSequence(&pool->sequences[p->index], pool->sequenceResource, sequence, 0,
                         (u8)(Mod(Random(table), 2) | flip));
        effect->flags |= 0x201;
        effect->priority = priority;
        p->state = 1;
        p->flags = flags;
    }
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08067E3C);
#endif
