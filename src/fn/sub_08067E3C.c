// MATCHING_CONCESSION: register-pinning
#include "scene_particle_burst.h"
#include "engine.h"
// confidence=low: spawns pooled particles with optional velocity ranges and position jitter.
#define SceneParticleBurst_CreateWithRanges sub_08067E3C
extern struct SceneParticleBurstPool *gUnk_02000130;
extern const u16 gUnk_0203B400[];
struct SceneParticleBurstPool *SceneParticleBurst_EnsureSingleton(u32, u32);
struct SceneParticleBurstSlot *SceneParticleBurst_AllocateSlot(struct SceneParticleBurstPool *);
s32 Mod(s32, s32);
void ParticleSprite_Initialize(void *, const void *, u32);
void Effect_SetOrigin(void *, s32, s32);
static inline u32 Random(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
static inline u8 SceneHasFlag(u32 mask)
{
    if (*(u32 *)((u8 *)gGameState + 0x868) & mask)
        return 1;
    return 0;
}
s32 SceneParticleBurst_CreateWithRanges(const struct ContactPosition *position,
                                        const struct ContactPosition *velocity0,
                                        const struct ContactPosition *jitter0, s32 count0,
                                        u32 priority, u32 flip, u32 flags)
{
    const struct ContactPosition *jitter = jitter0;
    s32 count = count0;
    struct SceneParticleBurstPool *pool;
    struct SceneParticleEffect *effect;
    s32 i;
    register const u16 *table asm("r10");
    if (!position)
        return -1;
    pool = gUnk_02000130;
    if (!pool) {
        pool = SceneParticleBurst_EnsureSingleton(0, 0);
        if (!pool)
            return -1;
    }
    i = 0;
    if (i < count) {
        table = gUnk_0203B400;
        do {
            register struct SceneParticleBurstSlot *p;
            s16 sequence;
            p = SceneParticleBurst_AllocateSlot(pool);
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
            if (!velocity0) {
                p->velocity.x = 0;
                p->velocity.y = 0;
                p->velocity.z = 0;
            } else {
                struct ContactPackedVector value;
                if (!velocity0->x)
                    value.x = 0;
                else
                    value.x = Mod(Random(table), velocity0->x);
                if (!velocity0->y)
                    value.y = 0;
                else
                    value.y = Mod(Random(table), velocity0->y);
                if (!velocity0->z)
                    value.z = 0;
                else
                    value.z = Mod(Random(table), velocity0->z);
                *((struct ContactPackedVector *)(&p->velocity)) = value;
            }
            p->delay = Mod(Random(table), 16);
            ParticleSprite_Initialize(effect, pool->graphicsResource, 0);
            Effect_SetOrigin(effect, -8, -8);
            sequence = SceneHasFlag(8) ? (14) : (13);
            Anim_SetSequence(&pool->sequences[p->index], pool->sequenceResource, sequence, 0,
                             (u8)(Mod(Random(table), 2) | flip));
            effect->flags |= 0x201;
            effect->priority = priority;
            p->state = 1;
            p->flags = flags;
        } while (++i < count);
    }

    return 0;
}
