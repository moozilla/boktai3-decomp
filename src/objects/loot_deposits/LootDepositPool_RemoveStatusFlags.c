// COMPILER: old_agbcc
#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"
/* LootDepositPool_RemoveStatusFlags: Removes bits from the status flags of a 0x184-byte pooled object */

void LootDepositPool_RemoveStatusFlags(struct CallbackPoolEntry *entry, u16 mask)
{
    entry->statusFlags &= ~mask;
}
