// MATCHING_CONCESSION: register-pinning
#include "scene_particle_pool.h"
extern struct SceneParticlePool *gUnk_02000134;
extern void *gUnk_08605B5C[];
static inline u8 Occupied(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}
#define SceneParticlePool_Destroy sub_080687EC
s32 SceneParticlePool_Destroy(struct SceneParticlePool *input)
{
    register struct SceneParticlePool *pool asm("r6") = input;
    struct SceneParticleSlot *slot;
    s32 i = 0;
    u32 *used = &pool->used;
    slot = pool->slots;
    for (i = 0; i < 8; slot++, i++) {
        if (Occupied(used, 1u << i)) {
            register s32 index asm("r1") = slot->index;
            Effect_Remove(&pool->effects[index]);
        }
    }
    {
        register u32 zero asm("r1") = 0;
        gUnk_02000134 = (void *)zero;
    }
    return 0;
}
