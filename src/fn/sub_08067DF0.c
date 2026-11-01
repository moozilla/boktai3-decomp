// MATCHING_CONCESSION: register-pinning
#include "scene_particle_burst.h"
static inline u8 IsUsed(u32 *used, u32 index, u32 one)
{
    if (*used & (one << index))
        return 1;
    return 0;
}
struct SceneParticleBurstSlot *SceneParticleBurst_AllocateSlot(struct SceneParticleBurstPool *p)
{
    s32 i = 0;
    u32 *used = &p->used;
    u32 one = 1;
    u32 offset = 0;
    for (; i < 32; offset += 0x18, i++) {
        if (!IsUsed(used, i, one)) {
            struct SceneParticleBurstSlot *slot = (void *)((u8 *)p->slots + offset);
            slot->index = i;
            {
                u32 mask = one;
                DEBUG_NOP();
                mask <<= i;
                *used |= mask;
            }
            return slot;
        }
    }
    return 0;
}
