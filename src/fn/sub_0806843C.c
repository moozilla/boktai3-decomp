#include "scene_particle_pool.h"
void SceneParticle_ReleaseSlot(struct SceneParticlePool *p, struct SceneParticleSlot *slot)
{
    Effect_Remove((u8 *)p + (slot->index * 0x28 + 0x20));
    slot->state = 0;
    {
        s32 index = slot->index;
        u32 *used = &p->used;
        *used &= ~(1U << index);
    }
    Memory_Clear(slot, sizeof(*slot));
    slot->index = -1;
}
