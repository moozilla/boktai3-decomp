#include "scene/collision/contact_swarm.h"

void CountBats_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
s32 CountBats_SpawnPairedAdvanceCount(struct ContactSwarm *pool,
                                      const struct ContactPosition *position, s32 direction,
                                      u32 speed, u32 resourceId, u32 cost, u32 option, s32 count)
{
    s32 headings[2];
    struct ContactSwarmSlot *slot;
    s32 index;
    struct ContactCollider *resource;
    headings[0] = direction - 96;
    headings[1] = direction + 96;
    slot = pool->slots;
    for (index = 0; index < count; index++, slot++) {
        if (!(pool->activeMask & (1 << index))) {
            const void *table;
            struct ContactCollider *collider;
            {
                u32 randomIndex;
                u32 firstWord;
                u32 secondWord;
                s32 heading;
                u32 headingIndex;
                slot->state = 1;
                slot->enter = 1;
                slot->stateFrame = 0;
                table = gRandomTable;
                randomIndex = (gRandomIndex + 1) & 1023;
                firstWord = ((const u16 *)table)[randomIndex];
                slot->advanceFrames = (firstWord & 31) + 4;
                slot->advanceSpeed = 20;
                slot->trackingFrames = index * 8 + 15;
                randomIndex++;
                randomIndex &= 1023;
                gRandomIndex = randomIndex;
                secondWord = ((const u16 *)table)[randomIndex];
                table = headings;
                headingIndex = index & 1;
                heading = ((const s32 *)table)[headingIndex];
                slot->direction = heading + (secondWord & 63) - 32;
                slot->speed = speed;
            }
            slot->display.animation.draw.sprite.position = *position;
            slot->display.animation.draw.sprite.flags &= ~1;
            CountBats_SetDirectionalSequence(pool, slot, 0);
            AnimData_SelectPaletteBank(&slot->display.animation.draw.graphics, 0x2D7);
            slot->display.animation.draw.graphics.paletteData = pool->palette;
            ActorShadow_Enable(slot->shadow);
            resource = &slot->resourceCollider;
            collider = resource;
            Collider_SetPosition(collider, &slot->display.animation.draw.sprite.position, 0);
            Collider_SetAttackProperties(collider, resourceId, cost, 0x8000, 0, option);
            Collider_SetPosition(&slot->display.healthCollider,
                                 &slot->display.animation.draw.sprite.position, 0);
            ContactCollider_SetFlags(&slot->display.healthCollider, 4);
            pool->activeMask |= 1 << index;
        }
    }
    {
        u8 *fadeActive = &pool->fadeActive;
        u32 zero = 0;
        void *first;
        void *second;
        *fadeActive = 1;
        pool->fadeLevel = 20;
        pool->fadeReverse = zero;
        first = gPaletteBankData + 0x5A60;
        second = gPaletteBankData + 0x5AE0;
        Palette_InterpolateBanks(pool->palette, first, second, pool->fadeLevel, 5);
    }
    return 0;
}
