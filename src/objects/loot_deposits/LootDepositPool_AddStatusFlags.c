#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"

void LootDepositPool_AddStatusFlags(struct CallbackPoolEntry *entry, u16 flags)
{
    entry->statusFlags |= flags;
}
