#include "global.h"
#include "ram.h"

#include "sprite_particle_pool.h"

void Sprite_Remove(void *);
static inline u8 Occupied(u32 *used, u32 mask)
{
    if (*used & mask)
        return 1;
    return 0;
}
s32 SpriteGroup_RemoveSprites(struct SpriteParticlePool *pool)
{
    s32 i = 0;
    u32 zero;
    u32 *used = &pool->occupied;
    struct SpriteParticleSlot *entry = pool->slots;
    for (; i < 8; entry++, i++) {
        if (Occupied(used, 1 << i))
            Sprite_Remove(&pool->sprites[entry->index]);
    }
    zero = 0;
    gSpriteGroup = (void *)zero;
    return 0;
}
