#include "contact_swarm.h"
#ifdef NONMATCHING
// fuzzy: 88.8%
// fix.py best version
// Only the speed-byte stack reload uses r0 instead of r2; the reviewed body is 428 bytes.
#define ContactSwarm_SpawnPairedAdvanceCount sub_08044AD4
void ContactSwarm_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
s32 ContactSwarm_SpawnPairedAdvanceCount(struct ContactSwarm *pool,
                                         const struct ContactPosition *position, s32 direction,
                                         u32 speed, u32 resourceId, u32 cost, volatile u32 option,
                                         s32 count)
{
    s32 headings[2];
    struct ContactSwarmSlot *slot;
    struct ContactSwarmDrawPacket *draw;
    s32 index;
    s32 limit;
    typeof(&slot->resourceCollider) t1;
    headings[0] = direction - 96;
    headings[1] = direction + 96;
    slot = &pool->slots[0];
    index = 0;
    if (index < count) {
        draw = &pool->slots[0].display.animation.draw;
        do {
            u32 initialMask = 1;
            u32 mask;
            u32 bits;
            s32 bitIndex;
            const struct ContactSwarm *current;
            u32 active;
            mask = initialMask;
            bits = mask;
            bitIndex = index;
            bits <<= bitIndex;
            mask = bits;
            current = pool;
            active = current->activeMask & bits;
            if (!active) {
                struct ContactPosition *destination;
                struct ContactCollider *collider;
                {
                    const u16 *table;
                    u32 randomIndex;
                    u32 firstWord;
                    u32 secondWord;
                    s32 heading;
                    u32 randomMask;
                    u32 *counter;
                    u32 headingIndex;
                    u32 state = 1;
                    slot->state = state;
                    slot->enter = state;
                    slot->stateFrame = active;
                    table = (const u16 *)0x0203B400;
                    counter = &gUnk_03005308;
                    randomIndex = (*counter) + 1;
                    randomMask = 1023;
                    randomIndex &= randomMask;
                    firstWord = table[randomIndex];
                    firstWord &= 31;
                    firstWord += 4;
                    slot->advanceFrames = firstWord;
                    slot->advanceSpeed = 20;
                    {
                        s32 stagger = index;
                        {
                            u32 frames = (stagger * 8) + 15;
                            slot->trackingFrames = frames;
                        }
                    }
                    randomIndex++;
                    randomMask = 1023;
                    randomIndex &= randomMask;
                    counter = &gUnk_03005308;
                    *counter = randomIndex;
                    secondWord = table[randomIndex];
                    headingIndex = index;
                    headingIndex &= 1;
                    heading = headings[headingIndex];
                    secondWord &= 63;
                    slot->direction = (heading + secondWord) - 32;
                    slot->speed = speed;
                }
                destination = &slot->display.animation.draw.sprite.position;
                {
                    const struct ContactPosition *source = position;
                    struct ContactPosition copy;
                    struct ContactSwarmDrawPacket *view;
                    copy = *source;
                    view = draw;
                    view->sprite.position = copy;
                    view->sprite.flags &= ~1;
                }
                ContactSwarm_SetDirectionalSequence(pool, slot, 0);
                AnimData_SelectPaletteBank(&slot->display.animation.draw.graphics, 0x2D7);
                {
                    void *palette = pool->palette;
                    struct ContactSwarmDrawPacket *view;
                    view = draw;
                    view->graphics.paletteData = palette;
                }
                {
                    u32 offset = ((u8 *)slot->shadow) - ((u8 *)slot);
                    ActorShadow_Enable(((u8 *)slot) + offset);
                }
                t1 = &slot->resourceCollider;
                collider = t1;
                Collider_SetPosition(collider, destination, 0);
                Collider_SetAttackProperties(collider, resourceId, cost, 0x8000, active, option);
                collider = &slot->display.healthCollider;
                Collider_SetPosition(collider, destination, 0);
                collider->flags |= 4;
                {
                    struct ContactSwarm *update = pool;
                    {
                        u32 bit;
                        u32 flags = update->activeMask;
                        bit = mask;
                        update->activeMask = flags | bit;
                    }
                }
            }
            {
                s32 nextIndex = index;
                u32 stride;
                nextIndex++;
                index = nextIndex;
                draw = (void *)(((u8 *)draw) + stride);
                stride = sizeof(*slot);
                slot = (void *)(((u8 *)slot) + stride);
            }
            limit = count;
        } while (index < limit);
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
        bank = gUnk_030042E4;
        first = bank + 0x5A60;
        second = (bank + 0x5A60) + 0x80;
        Palette_InterpolateBanks(pool->palette, first, second, *level, 5);
    }
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08044AD4);
#endif
