#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/sprite_drop.h"

struct SpriteDropPool *SpriteDrop_CreateManager(void)
{
    struct SpriteDropPool *pool;
    if (gCompactParticlePool != 0)
        return (struct SpriteDropPool *)gCompactParticlePool;
    pool = Task_Create(0x8, sizeof(*pool));
    if (pool != 0) {
        Task_SetCallbacks(pool, (void (*)(void *))CallbackPool_InvokeActiveSmall,
                          (void (*)(void *))CallbackPool_RemoveSmallSprites);
        if (ActorParticles_InitCompactPool(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
