#include "scene_particle_pool.h"
#include "ram.h"
extern void *gUnk_08605B5C[];
static inline u8 Occupied(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}
s32 SceneParticlePool_Destroy(struct SceneParticlePool *input)
{
    struct SceneParticlePool *pool = input;
    struct SceneParticleSlot *slot;
    s32 i = 0;
    u32 *used = &pool->used;
    slot = pool->slots;
    for (i = 0; i < 8; slot++, i++) {
        if (Occupied(used, 1u << i)) {
            s32 index = slot->index;
            Effect_Remove(&pool->effects[index]);
        }
    }
    {
        u32 zero = 0;
        gSceneParticleEmitterPool = (void *)zero;
    }
    return 0;
}
