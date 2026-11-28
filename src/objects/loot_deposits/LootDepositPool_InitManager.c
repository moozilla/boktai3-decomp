#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"

s32 LootDepositPool_InitManager(struct CallbackPool *pool)
{
    pool->sequenceResource = ScriptProc_Start(0x922E, 0x74C9);
    gActorGroupPool = pool;
    return pool->activeMask = 0;
}
