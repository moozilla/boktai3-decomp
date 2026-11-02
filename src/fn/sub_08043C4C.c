#include "contact_actor.h"
#include "player.h"
void ContactActor_InitializeSlot(struct ContactActorPool *, struct ContactActor *, s32);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
s32 ContactActorPool_Initialize(struct ContactActorPool *pool, u32 mode, u32 resourceId,
                                u32 healthId)
{
    union {
        struct ContactActor *slot;
        struct ContactActorPool *pool;
    } cursor;
    s32 index;
    cursor.pool = pool;
    cursor.pool->resourceColliderId = resourceId;
    cursor.pool->healthColliderId = healthId;
    cursor.pool->hideRequested = 0;
    cursor.pool->state = 0;
    cursor.pool->mode = mode;
    cursor.pool->sequenceResource = ScriptProc_Start(0x922E, 0x5BB7);
    {
        void *first = gPaletteBankData + 0x5A60;
        void *second = gPaletteBankData + 0x5AA0;
        Palette_InterpolateBanks(cursor.pool->tileData, first, second, 64, 6);
    }
    cursor.slot = cursor.pool->slots;
    index = 0;
    do {
        ContactActor_InitializeSlot(pool, cursor.slot, index);
        index++;
        cursor.slot = (void *)((u8 *)cursor.slot + sizeof(*cursor.slot));
    } while (index <= 15);
    index = 0;
    if (index < pool->mode) {
        const struct ContactPosition **target = pool->targets;
        struct Player **player = gPlayers;
        do {
            *target++ = (const struct ContactPosition *)&(*player++)->position;
        } while (++index < pool->mode);
    }
    return 0;
}
