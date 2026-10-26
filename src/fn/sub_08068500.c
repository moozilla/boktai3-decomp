#include "scene_particle_pool.h"
#include "engine.h"
// confidence=low: spawns pooled particles with optional velocity ranges and position jitter.
#define SceneParticle_CreateAnchoredRanges sub_08068500
#ifdef NONMATCHING
// fuzzy: 76.9%
// fix.py best (sweep after the inlined-insn fix)
// Remaining: packed velocity temporaries, loop and pointer/register lifetimes.
extern struct SceneParticlePool *gUnk_02000134;
extern const u16 gUnk_0203B400[];
struct SceneParticleSlot *SceneParticle_AllocateSlot(struct SceneParticlePool *);
s32 Mod(s32, s32);
void ParticleSprite_Initialize(void *, const void *, u32);
void Effect_SetOrigin(void *, s32, s32);
static inline u32 Random(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
s32 SceneParticle_CreateAnchoredRanges(const struct ContactPosition *position,
                                       const struct ContactPosition *offset,
                                       const struct ContactPosition *velocity,
                                       const struct ContactPosition *jitter, s32 count,
                                       u32 priority)
{
    struct SceneParticlePool *pool;
    s32 i;
    const u16 *table;
    if (!position)
        return -1;
    pool = gUnk_02000134;
    if (!pool)
        return -1;
    i = 0;
    table = gUnk_0203B400;
    for (; i < count; i++) {
        struct SceneParticleSlot *p = SceneParticle_AllocateSlot(pool);
        struct SceneParticleEffect *effect;
        if (!p)
            return -1;
        effect = &pool->effects[p->index];
        {
            typeof(p->anchor) *q1 = &p->anchor;
            u8 z1 = 0;
            *q1 = position;
            if (!offset) {
                p->offset.x = 0;
                p->offset.y = 0;
                p->offset.z = 0;
            } else
                p->offset = *offset;
            if (jitter) {
                if (jitter->x)
                    p->offset.x += Mod(Random(table), jitter->x * 2) - jitter->x;
                if (jitter->y)
                    p->offset.y += Mod(Random(table), jitter->y * 2) - jitter->y;
                if (jitter->z)
                    p->offset.z += Mod(Random(table), jitter->z * 2) - jitter->z;
            }
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
                *((struct ContactPackedVector *)(&p->velocity)) = value;
            }
            p->delay = z1;
            ParticleSprite_Initialize(effect, pool->graphicsResource, 0);
            Effect_SetOrigin(effect, -8, -8);
            Anim_SetSequence(&pool->sequences[p->index], pool->sequenceResource, 12, 0, 0);
            effect->flags |= 1;
            effect->priority = priority;
            p->state = 1;
        }
    }

    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08068500);
#endif
