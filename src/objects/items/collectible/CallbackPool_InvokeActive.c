#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/collectible.h"

s32 CallbackPool_InvokeActive(struct CollectiblePool *pool)
{
    s32 i;
    for (i = 0; i < COLLECTIBLE_COUNT; i++) {
        if (pool->items[i].active)
            Callback_InvokeActor(&pool->items[i], pool->items[i].update);
    }
    return 0;
}
