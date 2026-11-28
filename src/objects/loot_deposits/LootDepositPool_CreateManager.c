#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"

struct CallbackPool *LootDepositPool_CreateManager(void)
{
    struct CallbackPool *pool;
    if (gActorGroupPool != 0)
        return gActorGroupPool;
    pool = Task_Create(0xa, sizeof(*pool));
    if (pool != 0) {
        Task_SetCallbacks(pool, (void (*)(void *))LootDepositCallbackPool_UpdateEntries,
                          (void (*)(void *))LootDepositCallbackPool_DestroyEntries);
        if (LootDepositPool_InitManager(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
