#include "contact_swarm.h"
void ContactSwarm_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
static inline s32 ContactSwarm_ActivateChargeSlot(struct ContactSwarmSlot *slots,
                                                  struct ContactSwarm *poolInput,
                                                  const struct ContactPosition *positionInput,
                                                  u32 directionInput, u32 speedInput,
                                                  u32 resourceId, u32 cost, u32 option)
{
    struct ContactSwarm *pool = poolInput;
    const struct ContactPosition *position = positionInput;
    u32 direction = directionInput;
    u32 speed = speedInput;
    struct ContactSwarmSlot *slot = slots;
    struct ContactCollider *health;
    struct ContactPosition *destination;
    s32 index;
    u32 one;
    index = 0;
    one = 1;
    health = &pool->slots[0].display.healthCollider;
    destination = &pool->slots[0].display.animation.draw.sprite.position;
    do {
        u32 mask = one;
        u32 active;
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
            AnimData_SelectPaletteBank(&slot->display.animation.draw.graphics, 0x2D3);
            {
                struct ContactSwarmColliderAnimation *display = (void *)health;
                display->animation.draw.sprite.flags &= ~1;
            }
            ContactSwarm_SetDirectionalSequence(pool, slot, 0);
            {
                u32 offset = ((u8 *)slot->shadow) - ((u8 *)slot);
                ActorShadow_Enable(((u8 *)slot) + offset);
            }
            {
                struct ContactCollider *resource = &slot->resourceCollider;
                Collider_SetPosition(resource, destination, 0);
                Collider_SetAttackProperties(resource, resourceId, cost, 0x8000, active, option);
            }
            Collider_SetPosition(health, destination, 0);
            health->flags |= 4;
            {
                struct ContactSwarm *current = pool;
                u32 flags;
                u32 bit;
                flags = current->activeMask;
                bit = mask;
                current->activeMask = flags | bit;
            }
            goto done;
        }
    next:
        index++;

        health = (void *)(((u8 *)health) + (sizeof(*slot)));
        destination = (void *)(((u8 *)destination) + (sizeof(*slot)));
        slot++;
    } while (index <= 15);
done:
    return 0;
}

s32 ContactSwarm_SpawnCharge(struct ContactSwarm *poolInput,
                             const struct ContactPosition *positionInput, u32 directionInput,
                             u32 speedInput, u32 resourceId, u32 cost, u32 option)
{
    return ContactSwarm_ActivateChargeSlot(poolInput->slots, poolInput, positionInput,
                                           directionInput, speedInput, resourceId, cost, option);
}
