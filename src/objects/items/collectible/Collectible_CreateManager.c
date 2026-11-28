#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/collectible.h"

struct CollectiblePool *Collectible_CreateManager(void)
{
    struct CollectiblePool *pool;
    if (gActorParticlePool != 0)
        return (struct CollectiblePool *)gActorParticlePool;
    pool = Task_Create(0x8, sizeof(*pool));
    if (pool != 0) {
        Task_SetCallbacks(pool, (void (*)(void *))CallbackPool_InvokeActive,
                          (void (*)(void *))Collectible_DestroyManager);
        if (ActorParticles_InitPool(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
