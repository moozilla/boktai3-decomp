#include "scene_particle_burst.h"
static inline u8 IsUsed(u32 *used, s32 index)
{
    if (*used & (1 << index))
        return 1;
    return 0;
}
struct SceneParticleBurstSlot *SceneParticleBurst_AllocateSlot(struct SceneParticleBurstPool *p)
{
    s32 i;
    for (i = 0; i < 32; i++) {
        if (!IsUsed(&p->used, i)) {
            struct SceneParticleBurstSlot *slot = &p->slots[i];
            slot->index = i;
            p->used |= 1 << i;
            return slot;
        }
    }
    return 0;
}
