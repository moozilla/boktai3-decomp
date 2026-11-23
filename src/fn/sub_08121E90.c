#include "global.h"
#include "ram.h"
#include "sprite_particle_pool.h"

void Sprite_Remove(void *);

static inline u8 Occupied(u32 *used, s32 index)
{
    if (*used & (1 << index))
        return 1;
    return 0;
}

s32 SpriteGroup_RemoveSprites(struct SpriteParticlePool *pool)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (Occupied(&pool->occupied, i)) {
            struct SpriteParticleSlot *slot = &pool->slots[i];
            Sprite_Remove(&pool->sprites[slot->index]);
        }
    }
    gSpriteGroup = NULL;
    return 0;
}
