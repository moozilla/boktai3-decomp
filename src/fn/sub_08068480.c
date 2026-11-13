#include "scene_particle_pool.h"
static inline u8 IsUsed(u32 *used, s32 index)
{
    if (*used & (1 << index))
        return 1;
    return 0;
}
struct SceneParticleSlot *SceneParticle_AllocateSlot(struct SceneParticlePool *p)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (!IsUsed(&p->used, i)) {
            struct SceneParticleSlot *slot = &p->slots[i];
            slot->index = i;
            p->used |= 1 << i;
            return slot;
        }
    }
    return 0;
}
