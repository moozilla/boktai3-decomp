#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"

void sub_081047B4(void *unused, struct CallbackPoolEntry *entry)
{
    if (entry->defeated) {
        entry->frame = 0;
        entry->sprite.flags &= ~1;
        entry->defeated = 0;
    }
    sub_08104748(entry);
    LootDepositCallbackPool_UpdateShake(entry);
    if (entry->frame & 1)
        LootDepositFeedback_CheckPlayerRange(entry);
    entry->frame++;
}
