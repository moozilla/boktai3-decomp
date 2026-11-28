#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"

struct CallbackPoolEntry *sub_08104280(void *owner)
{
    struct CallbackPool *pool = gActorGroupPool;
    s32 i;
    if (!pool)
        return NULL;
    for (i = 0; i < 4; i++) {
        if (LootDepositCallbackPool_IsActive(&pool->activeMask, i)) {
            struct CallbackPoolEntry *entry = &pool->entries[i];
            if (entry->healthDisplay == owner)
                return entry;
        }
    }
    return NULL;
}
