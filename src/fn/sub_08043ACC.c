#include "contact_actor.h"
void ContactActorPool_HideSlots(struct ContactActorPool *);
void ContactActor_UpdateDisplay(struct ContactActor *);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
extern void (*const gUnk_086058FC[])(struct ContactActorPool *, struct ContactActor *, s32);
u32 ContactActorPool_Update(struct ContactActorPool *poolInput)
{
    struct ContactActorPool *pool = poolInput;
    if (pool->state) {
        s32 fade;
        if (pool->lifetime > 599) {
            if (!pool->clearRequested) {
                pool->clearRequested = 1;
                Sound_PlaySong(0xEA);
            }
            pool->fadeMode = 2;
        }
        fade = pool->fadeMode;
        if (fade == 0) {
            if (++pool->fadeLevel > 62) {
                pool->fadeLevel = 63;
                pool->fadeMode = -1;
            }
        } else if (fade == 1) {
            if (--pool->fadeLevel <= 0) {
                pool->fadeLevel = 0;
                pool->fadeMode = -1;
            }
        } else if (fade == 2) {
            if (++pool->fadeLevel > 31) {
                pool->fadeLevel = 32;
                pool->fadeMode = -1;
            }
        }
        if (pool->fadeMode >= 0) {
            u32 firstOffset = 0x5A60;
            u32 secondOffset = 0x5AA0;
            void *first = gPaletteBankData + firstOffset;
            void *second = gPaletteBankData + secondOffset;
            Palette_InterpolateBanks((u8 *)pool + 0x1700, first, second, pool->fadeLevel, 6);
        }
        if (pool->hideRequested) {
            ContactActorPool_HideSlots(pool);
        } else {
            s32 ready = 0;
            struct ContactActor *slot = &pool->slots[0];
            s32 index = 0;
            do {
                gUnk_086058FC[slot->state](pool, slot, index);
                ContactActor_UpdateDisplay(slot);
                if (slot->state == 7)
                    ready++;
                index++;
                slot++;
            } while (index <= 15);
            {
                u32 lifetime = pool->lifetime;
                if (ready == 16 || lifetime > 1199) {
                    if (!pool->hideRequested)
                        pool->hideRequested = 1;
                }
                pool->lifetime = lifetime + 1;
            }
        }
    }
    return 0;
}
