#include "global.h"
#include "sprite_particle_pool.h"

static inline u8 IsUsed(u32 *used, s32 index)
{
    if (*used & (1 << index))
        return 1;
    return 0;
}

struct SpriteParticleSlot *SpriteParticle_AllocateSlot_08121D14(struct SpriteParticlePool *pool)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (!IsUsed(&pool->occupied, i)) {
            struct SpriteParticleSlot *slot = &pool->slots[i];
            slot->index = i;
            pool->occupied |= 1 << i;
            return slot;
        }
    }
    return NULL;
}
