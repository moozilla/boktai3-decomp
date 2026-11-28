#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"

void ScaledShadow_Destroy(void *);
void Sprite_Remove(void *);
void Collider_Remove(void *);

void LootDeposit_UpdateMemberRemoval(struct CallbackPool *pool, struct CallbackPoolEntry *entry)
{
    if (entry->defeated) {
        LootDeposit_EmitRemovalDebris(entry);
        if (entry->spawnTarget - entry->spawnIndex >= 0)
            Sound_PlaySong(0x3C7);
        LootDepositFeedback_SetState(entry, entry->spawnTarget - entry->spawnIndex);
        if (entry->effectIndex != -1) {
            ScaledShadow_Destroy(&entry->shadow);
            entry->effectIndex = -1;
        }
        Sound_PlaySong(0x3C6);
        LootDeposit_CallMemberScript(entry);
        entry->defeated = 0;
    } else {
        entry->sprite.flags |= 1;
        Sprite_Remove(entry);
        Collider_Remove(&entry->collider);
        entry->callbackIndex = 0;
        {
            s32 slot = entry->slot;
            pool->activeMask &= ~(1 << slot);
        }
        entry->slot = -1;
    }
}
