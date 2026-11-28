#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/sprite_drop.h"

void Sprite_Remove(void *);

s32 CallbackPool_RemoveSmallSprites(struct SpriteDropPool *pool)
{
    s32 i;
    for (i = 0; i < SPRITE_DROP_COUNT; i++) {
        if (pool->items[i].sprite.unknown4[0])
            Sprite_Remove(&pool->items[i].sprite);
    }
    return (s32)(gCompactParticlePool = 0);
}
