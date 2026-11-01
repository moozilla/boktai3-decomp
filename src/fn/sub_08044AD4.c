#include "contact_swarm.h"

void ContactSwarm_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
s32 ContactSwarm_SpawnPairedAdvanceCount(struct ContactSwarm *pool,
                                         const struct ContactPosition *position, s32 direction,
                                         u32 speed, u32 resourceId, u32 cost, u32 option, s32 count)
{
    s32 headings[2];
    struct ContactSwarmSlot *slot;
    struct ContactSwarmDrawPacket *draw;
    s32 index;
    u32 stride = sizeof(*slot);
    struct ContactCollider *resource;
    headings[0] = direction - 96;
    headings[1] = direction + 96;
    slot = &pool->slots[0];
    index = 0;
    if (index < count) {
        draw = &pool->slots[0].display.animation.draw;
        do {
            u32 mask = 1U << index;
            u32 active = pool->activeMask & mask;
            if (!active) {
                const void *table;
                struct ContactPosition *destination;
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
                destination = &slot->display.animation.draw.sprite.position;
                draw->sprite.position = *position;
                draw->sprite.flags &= ~1;
                ContactSwarm_SetDirectionalSequence(pool, slot, 0);
                AnimData_SelectPaletteBank(&slot->display.animation.draw.graphics, 0x2D7);
                draw->graphics.paletteData = pool->palette;
                ActorShadow_Enable(slot->shadow);
                resource = &slot->resourceCollider;
                collider = resource;
                Collider_SetPosition(collider, destination, 0);
                Collider_SetAttackProperties(collider, resourceId, cost, 0x8000, 0, option);
                collider = &slot->display.healthCollider;
                Collider_SetPosition(collider, destination, 0);
                collider->flags |= 4;
                pool->activeMask |= mask;
            }
            index++;
            draw = (void *)((u8 *)draw + stride);
            slot++;
        } while (index < count);
    }
    {
        u8 *fadeActive = &pool->fadeActive;
        u32 zero = 0;
        s8 *level;
        u8 *reverse;
        u8 *bank;
        void *first;
        void *second;
        *fadeActive = 1;
        level = &pool->fadeLevel;
        *level = 20;
        reverse = &pool->fadeReverse;
        *reverse = zero;
        bank = gPaletteBankData;
        first = bank + 0x5A60;
        second = bank + 0x5AE0;
        Palette_InterpolateBanks(pool->palette, first, second, *level, 5);
    }
    return 0;
}
