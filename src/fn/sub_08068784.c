#include "scene_particle_pool.h"
#include "ram.h"
extern void *gUnk_08605B5C[];
static inline u8 Occupied(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}
static inline u32 *SlotMask(struct SceneParticlePool *pool)
{
    return &pool->used;
}
s32 SceneParticlePool_Update(struct SceneParticlePool *pool)
{
    s32 i;
    struct SceneParticleSlot *slot;
    u32 *used;
    void **table;
    if (!pool->used)
        return 0;
    i = 0;
    used = SlotMask(pool);
    slot = pool->slots;
    table = gUnk_08605B5C;
    for (; i < 8; slot++, i++) {
        if (Occupied(used, 1u << i))
            sub_08249240(pool, slot, table[slot->state]);
    }
    return 0;
}
