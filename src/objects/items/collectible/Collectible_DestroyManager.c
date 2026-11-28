#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/collectible.h"

void Sprite_Remove(void *);

u32 Collectible_DestroyManager(struct CollectiblePool *pool)
{
    s32 i;
    for (i = 0; i < COLLECTIBLE_COUNT; i++)
        Sprite_Remove(&pool->items[i].sprite);
    gActorParticlePool = 0;
    return 0;
}
