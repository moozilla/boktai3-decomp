// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
/* ContactSwarm_SpawnPairedAdvance: Fills free slots in the advance state with alternating paired headings and randomized timing then starts palette fading. */
#define ContactSwarm_SpawnPairedAdvance sub_08044934
#define ContactSwarm_SpawnPairedAdvance sub_08044934
void ContactSwarm_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
s32 ContactSwarm_SpawnPairedAdvance(struct ContactSwarm *pool,
                                    const struct ContactPosition *position, s32 direction,
                                    u32 speed, u32 resourceId, u32 cost, volatile u32 option)
{
    s32 headings[2];
    struct ContactSwarmSlot *slot;
    struct ContactSwarmDrawPacket *draw;
    s32 index;
    headings[0] = direction - 96;
    headings[1] = direction + 96;
    slot = &pool->slots[0];
    index = 0;
    draw = &pool->slots[0].display.animation.draw;
    do {
        register u32 initialMask asm("r2") = 1;
        register u32 mask asm("r10");
        u32 bits;
        register s32 bitIndex asm("r1");
        register const struct ContactSwarm *current asm("r2");
        u32 active;
        asm("" : "+r"(initialMask));
        mask = initialMask;
        asm("" : "+r"(mask));
        bits = mask;
        asm("" : "+r"(bits));
        bitIndex = index;
        asm("" : "+r"(bitIndex));
        bits <<= bitIndex;
        mask = bits;
        current = pool;
        asm("" : "+r"(current));
        active = current->activeMask & bits;
        if (!active) {
            register struct ContactPosition *destination asm("r5");
            struct ContactCollider *collider;
            {
                register const u16 *table asm("r3");
                register u32 randomIndex asm("r1");
                u32 firstWord;
                u32 secondWord;
                s32 heading;
                register u32 randomMask asm("r0");
                register u32 *counter asm("r2");
                u32 headingIndex;
                register u32 state asm("r0") = 1;
                slot->state = state;
                slot->enter = state;
                slot->stateFrame = active;
                table = (const u16 *)RANDOM_TABLE_ADDR;
                counter = &gRandomIndex;
                randomIndex = *counter + 1;
                randomMask = 1023;
                randomIndex &= randomMask;
                firstWord = table[randomIndex];
                firstWord &= 31;
                firstWord += 4;
                slot->advanceFrames = firstWord;
                slot->advanceSpeed = 20;
                {
                    register s32 stagger asm("r2") = index;
                    asm("" : "+r"(stagger));
                    {
                        register u32 frames asm("r0") = stagger * 8 + 15;
                        slot->trackingFrames = frames;
                    }
                }
                randomIndex++;
                randomMask = 1023;
                randomIndex &= randomMask;
                counter = &gRandomIndex;
                *counter = randomIndex;
                secondWord = table[randomIndex];
                headingIndex = index;
                asm("" : "+r"(headingIndex));
                headingIndex &= 1;
                heading = headings[headingIndex];
                secondWord &= 63;
                slot->direction = heading + secondWord - 32;
                slot->speed = speed;
            }
            destination = &slot->display.animation.draw.sprite.position;
            {
                const struct ContactPosition *source = position;
                struct ContactPosition copy;
                struct ContactSwarmDrawPacket *view;
                copy = *source;
                view = draw;
                asm("" : "+r"(view));
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
                u32 offset = (u8 *)slot->shadow - (u8 *)slot;
                ActorShadow_Enable((u8 *)slot + offset);
            }
            collider = &slot->resourceCollider;
            Collider_SetPosition(collider, destination, 0);
            Collider_SetAttackProperties(collider, resourceId, cost, 0x8000, active, option);
            collider = &slot->display.healthCollider;
            Collider_SetPosition(collider, destination, 0);
            collider->flags |= 4;
            {
                register struct ContactSwarm *update asm("r1") = pool;
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
            register u32 stride asm("r1");
            nextIndex++;
            index = nextIndex;
            stride = sizeof(*slot);
            draw = (void *)((u8 *)draw + stride);
            slot = (void *)((u8 *)slot + stride);
        }
    } while (index <= 15);
    {
        u8 *fadeActive = &pool->fadeActive;
        u32 zero = 0;
        s8 *level;
        u8 *reverse;
        u8 *bank;
        void *first, *second;
        *fadeActive = 1;
        level = &pool->fadeLevel;
        *level = 20;
        reverse = &pool->fadeReverse;
        *reverse = zero;
        bank = gPaletteBankData;
        first = bank + 0x5A60;
        second = bank + 0x5A60 + 0x80;
        Palette_InterpolateBanks(pool->palette, first, second, *level, 5);
    }
    return 0;
}
