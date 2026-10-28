// MATCHING_CONCESSION: register-pinning
#include "contact_actor.h"
#include "player.h"
void ContactActor_InitializeSlot(struct ContactActorPool *, struct ContactActor *, s32);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
s32 ContactActorPool_Initialize(struct ContactActorPool *pool, u32 mode, u32 resourceId,
                                u32 healthId)
{
    struct ContactActor *slot;
    register s32 index asm("r5");
    u32 hide = 0;
    u32 zero;
    asm("" : "+r"(hide));
    zero = 0;
    pool->resourceColliderId = resourceId;
    pool->healthColliderId = healthId;
    pool->hideRequested = hide;
    pool->state = zero;
    pool->mode = mode;
    pool->sequenceResource = ScriptProc_Start(0x922E, 0x5BB7);
    {
        u8 *bank = gPaletteBankData;
        void *first = bank + 0x5A60;
        void *second = bank + 0x5AA0;
        Palette_InterpolateBanks((u8 *)pool + 0x1700, first, second, 64, 6);
    }
    slot = &pool->slots[0];
    index = 0;
    do {
        ContactActor_InitializeSlot(pool, slot, index);
        index++;
        slot++;
    } while (index <= 15);
    index = 0;
    if (index < pool->mode) {
        const struct ContactPosition **target = pool->targets;
        struct Player **player = gPlayers;
        do {
            *target++ = (const void *)((u8 *)*player++ + 0x30);
            index++;
        } while (index < pool->mode);
    }
    return 0;
}
