#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"

u8 LootDepositPool_TestStatusFlags(struct CallbackPoolEntry *entry, u32 mask)
{
    if (entry->statusFlags & mask)
        return 1;
    return 0;
}
