#include "scene_particle_pool.h"
#include "engine.h"
#include "ram.h"
// confidence=low: spawns pooled particles with optional velocity ranges and position jitter.
#define SceneParticle_CreateAnchoredRanges sub_08068500
struct SceneParticleSlot *SceneParticle_AllocateSlot(struct SceneParticlePool *);
s32 Mod(s32, s32);
void ParticleSprite_Initialize(void *, const void *, u32);
void Effect_SetOrigin(void *, s32, s32);
static inline u32 ParticleRandom_NextByteOffset(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gRandomIndex * 2;
}
#define ParticleRandom_ReadSample(table)                                                           \
    (*(const u16 *)(ParticleRandom_NextByteOffset() + (u32)(table)))

s32 SceneParticle_CreateAnchoredRanges(const struct ContactPosition *position,
                                       const struct ContactPosition *offset,
                                       const struct ContactPosition *velocity,
                                       const struct ContactPosition *jitter, s32 count,
                                       u32 priority)
{
    struct SceneParticlePool *pool;
    struct SceneParticleEffect *effect;
    struct {
        s32 i;
        const u16 *table;
    } iteration;
    if (!position)
        return -1;
    pool = gSceneParticleEmitterPool;
    if (!pool)
        return -1;
    iteration.i = 0;
    if (iteration.i < count) {
        iteration.table = gRandomTable;
        do {
            struct SceneParticleSlot *p = SceneParticle_AllocateSlot(pool);
            if (!p)
                return -1;
            effect = &pool->effects[p->index];
            {
                p->anchor = position;
                if (!offset) {
                    p->offset.x = 0;
                    p->offset.y = 0;
                    p->offset.z = 0;
                } else
                    p->offset = *offset;
                if (jitter) {
                    if (jitter->x)
                        p->offset.x +=
                            Mod(ParticleRandom_ReadSample(iteration.table), jitter->x * 2) -
                            jitter->x;
                    if (jitter->y)
                        p->offset.y +=
                            Mod(ParticleRandom_ReadSample(iteration.table), jitter->y * 2) -
                            jitter->y;
                    if (jitter->z)
                        p->offset.z +=
                            Mod(ParticleRandom_ReadSample(iteration.table), jitter->z * 2) -
                            jitter->z;
                }
                if (!velocity) {
                    p->velocity.x = 0;
                    p->velocity.y = 0;
                    p->velocity.z = 0;
                } else {
                    union {
                        struct ContactPosition position;
                        struct {
                            u32 xy, zPad;
                        } words;
                    } value;
                    if (!velocity->x)
                        value.position.x = 0;
                    else
                        value.position.x =
                            Mod(ParticleRandom_ReadSample(iteration.table), velocity->x);
                    if (!velocity->y)
                        value.position.y = 0;
                    else
                        value.position.y =
                            Mod(ParticleRandom_ReadSample(iteration.table), velocity->y);
                    {
                        u32 operand;
                        if (!velocity->z) {
                            operand = 0xffff0000;
                            value.words.zPad &= operand;
                        } else {
                            operand = ParticleRandom_ReadSample(iteration.table);
                            value.position.z = Mod(operand, velocity->z);
                        }
                    }
                    p->velocity = value.position;
                }
                p->delay = 0;
                ParticleSprite_Initialize(effect, pool->graphicsResource, 0);
                Effect_SetOrigin(effect, -8, -8);
                Anim_SetSequence(&pool->sequences[p->index], pool->sequenceResource, 12, 0, 0);
                effect->flags |= 1;
                effect->priority = priority;
                p->state = 1;
            }
        } while (++iteration.i < count);
    }

    return 0;
}
