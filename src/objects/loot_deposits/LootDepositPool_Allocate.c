#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"

struct CallbackPoolEntry *LootDepositPool_Allocate(struct CallbackPool *pool)
{
    s32 i;
    struct CallbackPoolEntry *entry;
    for (i = 0; i < 4; i++) {
        if (!LootDepositCallbackPool_IsActive(&pool->activeMask, i)) {
            entry = &pool->entries[i];
            pool->activeMask |= 1 << i;
            Memory_Clear(entry, sizeof(*entry));
            entry->slot = i;
            return entry;
        }
    }
    return 0;
}
