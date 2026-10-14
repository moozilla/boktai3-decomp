// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#define ContactSwarm_SpawnCharge            sub_08044854
#define ContactSwarm_SetDirectionalSequence sub_08044174
void ContactSwarm_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
void sub_0821FF58(void *, u32, u32, u32, u32, u32);
s32 ContactSwarm_SpawnCharge(struct ContactSwarm *poolInput,
                             const struct ContactPosition *positionInput, u32 directionInput,
                             u32 speedInput, u32 resourceId, u32 cost, volatile u32 option)
{
    struct ContactSwarm *pool = poolInput;
    const struct ContactPosition *position = positionInput;
    register u32 speed asm("r10") = speedInput;
    register u32 direction asm("r2") = directionInput;
    struct ContactSwarmSlot *slot = pool->slots;
    register struct ContactCollider *health asm("r6");
    struct ContactPosition *destination;
    register s32 index asm("r0");
    register u32 one asm("r1");
    index = 0;
    one = 1;
    health = &pool->slots[0].display.healthCollider;
    destination = &pool->slots[0].display.animation.draw.sprite.position;
    do {
        register u32 mask asm("r8") = one;
        u32 active;
        asm("" : "+r"(mask));
        mask <<= index;
        active = pool->activeMask & mask;
        if (active)
            goto next;
        {
            slot->state = 3;
            slot->enter = one;
            slot->stateFrame = active;
            slot->direction = direction;
            slot->speed = speed;
            {
                const struct ContactPosition *current = position;
                *destination = *current;
            }
            AnimData_SelectTiles(&slot->display.animation.draw.graphics, 0x2D3);
            {
                register struct ContactSwarmColliderAnimation *display asm("r6") = (void *)health;
                display->animation.draw.sprite.flags &= ~1;
            }
            ContactSwarm_SetDirectionalSequence(pool, slot, 0);
            {
                register u32 offset asm("r3") = (u8 *)slot->shadow - (u8 *)slot;
                ActorShadow_Enable((u8 *)slot + offset);
            }
            {
                struct ContactCollider *resource = &slot->resourceCollider;
                Collider_SetPosition(resource, destination, 0);
                sub_0821FF58(resource, resourceId, cost, 0x8000, active, option);
            }
            Collider_SetPosition(health, destination, 0);
            health->flags |= 4;
            {
                register struct ContactSwarm *current asm("r1") = pool;
                u32 flags;
                u32 bit;
                flags = current->activeMask;
                bit = mask;
                asm("" : "+r"(bit));
                current->activeMask = flags | bit;
            }
            goto done;
        }
    next:
        index++;
        health = (void *)((u8 *)health + sizeof(*slot));
        destination = (void *)((u8 *)destination + sizeof(*slot));
        slot++;
    } while (index <= 15);
done:
    return 0;
}
