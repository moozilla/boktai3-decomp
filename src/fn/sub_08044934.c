#include "contact_swarm.h"
#ifdef NONMATCHING
// fuzzy: 93.1%
// MATCHING_CONCESSION: register-pinning
// Reviewed paired-spawn behavior; allocation and branch encoding still differ.
#define ContactSwarm_SpawnPairedAdvance sub_08044934
void ContactSwarm_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
void sub_0821FF58(void *, u32, u32, u32, u32, u32);
void sub_08221110(void *, void *, void *, s32, s32);
s32 ContactSwarm_SpawnPairedAdvance(struct ContactSwarm *pool,
                                    const struct ContactPosition *position, s32 direction,
                                    u32 speed, u32 resourceId, u32 cost, volatile u32 option)
{
    s32 headings[2];
    struct ContactSwarmSlot *slot;
    register struct ContactSwarmDrawPacket *draw asm("r9");
    s32 index;
    headings[0] = direction - 96;
    headings[1] = direction + 96;
    slot = &pool->slots[0];
    index = 0;
    draw = &pool->slots[0].display.animation.draw;
    do {
        register u32 initialMask asm("r2") = 1;
        register u32 mask asm("r10");
        register u32 bits asm("r0");
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
        asm("" : "+r"(mask));
        current = pool;
        asm("" : "+r"(current));
        active = current->activeMask & bits;
        if (!active) {
            register struct ContactPosition *destination asm("r5");
            struct ContactCollider *collider;
            {
                register const u16 *table asm("r3");
                register u32 randomIndex asm("r1");
                register u32 firstWord asm("r0");
                register u32 secondWord asm("r2");
                register s32 heading asm("r1");
                register u32 randomMask asm("r0");
                register u32 *counter asm("r2");
                register u32 headingIndex asm("r0");
                register u32 state asm("r0") = 1;
                asm("" : "+r"(state));
                slot->state = state;
                slot->enter = state;
                slot->stateFrame = active;
                table = (const u16 *)RANDOM_TABLE_ADDR;
                asm("" : : "r"(table));
                counter = &gUnk_03005308;
                asm("" : "+r"(counter));
                randomIndex = *counter + 1;
                asm("" : "+r"(randomIndex));
                randomMask = 1023;
                asm("" : "+r"(randomMask));
                randomIndex &= randomMask;
                firstWord = table[randomIndex];
                asm("" : "+r"(firstWord));
                firstWord &= 31;
                firstWord += 4;
                asm("" : "+r"(firstWord));
                slot->advanceFrames = firstWord;
                slot->advanceSpeed = 20;
                {
                    register s32 stagger asm("r2") = index;
                    asm("" : "+r"(stagger));
                    {
                        register u32 frames asm("r0") = stagger * 8 + 15;
                        asm("" : "+r"(frames));
                        slot->trackingFrames = frames;
                    }
                }
                randomIndex++;
                asm("" : "+r"(randomIndex));
                randomMask = 1023;
                asm("" : "+r"(randomMask));
                randomIndex &= randomMask;
                counter = &gUnk_03005308;
                asm("" : : "r"(counter));
                *counter = randomIndex;
                secondWord = table[randomIndex];
                asm("" : "+r"(secondWord));
                headingIndex = index;
                asm("" : "+r"(headingIndex));
                headingIndex &= 1;
                asm("" : "+r"(headingIndex));
                heading = headings[headingIndex];
                secondWord &= 63;
                asm("" : "+r"(secondWord));
                slot->direction = heading + secondWord - 32;
                slot->speed = speed;
            }
            destination = &slot->display.animation.draw.sprite.position;
            {
                register const struct ContactPosition *source asm("r2") = position;
                struct ContactPosition copy;
                register struct ContactSwarmDrawPacket *view asm("r2");
                asm("" : "+r"(source));
                copy = *source;
                view = draw;
                asm("" : "+r"(view));
                view->sprite.position = copy;
                view->sprite.flags &= ~1;
            }
            ContactSwarm_SetDirectionalSequence(pool, slot, 0);
            AnimData_SelectTiles(&slot->display.animation.draw.graphics, 0x2D7);
            {
                register void *palette asm("r0") = pool->palette;
                register struct ContactSwarmDrawPacket *view asm("r1");
                asm("" : "+r"(palette));
                view = draw;
                view->graphics.paletteData = palette;
            }
            {
                register u32 offset asm("r2") = (u8 *)slot->shadow - (u8 *)slot;
                asm("" : "+r"(offset));
                ActorShadow_Enable((u8 *)slot + offset);
            }
            collider = &slot->resourceCollider;
            Collider_SetPosition(collider, destination, 0);
            sub_0821FF58(collider, resourceId, cost, 0x8000, active, option);
            collider = &slot->display.healthCollider;
            Collider_SetPosition(collider, destination, 0);
            collider->flags |= 4;
            {
                register struct ContactSwarm *update asm("r1") = pool;
                asm("" : "+r"(update));
                {
                    register u32 bit asm("r2");
                    register u32 flags asm("r0") = update->activeMask;
                    asm("" : "+r"(flags));
                    bit = mask;
                    asm("" : "+r"(bit));
                    update->activeMask = flags | bit;
                }
            }
        }
        {
            register s32 nextIndex asm("r0") = index;
            register u32 stride asm("r1");
            asm("" : "+r"(nextIndex));
            nextIndex++;
            index = nextIndex;
            stride = sizeof(*slot);
            asm("" : "+r"(stride));
            draw = (void *)((u8 *)draw + stride);
            slot = (void *)((u8 *)slot + stride);
        }
    } while (index <= 15);
    {
        register u8 *fadeActive asm("r1") = &pool->fadeActive;
        register u32 zero asm("r2") = 0;
        register s8 *level asm("r3");
        register u8 *reverse asm("r0");
        u8 *bank;
        void *first, *second;
        asm("" : "+r"(zero));
        *fadeActive = 1;
        level = &pool->fadeLevel;
        *level = 20;
        reverse = &pool->fadeReverse;
        *reverse = zero;
        bank = gUnk_030042E4;
        first = bank + 0x5A60;
        second = bank + 0x5A60 + 0x80;
        asm("" : "+r"(first), "+r"(second));
        sub_08221110(pool->palette, first, second, *level, 5);
    }
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08044934);
#endif
