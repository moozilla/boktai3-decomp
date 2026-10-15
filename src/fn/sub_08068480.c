// MATCHING_CONCESSION: register-pinning
#include "scene_particle_pool.h"
#define SceneParticle_AllocateSlot sub_08068480
static inline u8 IsUsed(u32 *used, u32 index, u32 one)
{
    if (*used & (one << index))
        return 1;
    return 0;
}
struct SceneParticleSlot *SceneParticle_AllocateSlot(struct SceneParticlePool *input)
{
    register struct SceneParticlePool *p asm("r2") = input;
    s32 i = 0;
    u32 *used = &p->used;
    u32 one = 1;
    u32 offset = i;
    for (; i < 8; offset += sizeof(struct SceneParticleSlot), i++) {
        if (!IsUsed(used, i, one)) {
            struct SceneParticleSlot *slot = (void *)((u8 *)p->slots + offset);
            slot->index = i;
            {
                register u32 mask asm("r2") = one;
                mask <<= i;
                *used |= mask;
            }
            return slot;
        }
    }
    return 0;
}
