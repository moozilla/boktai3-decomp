// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
void ContactSwarm_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
void ContactSwarm_UpdateTracking(struct ContactSwarm *pool, struct ContactSwarmSlot *slot,
                                 s32 index)
{
    struct ContactSwarmSprite *sprite = &slot->display.animation.draw.sprite;
    struct ContactPosition *position;
    if (slot->enter) {
        struct ContactCollider *collider;
        s32 mask;
        slot->enter = 0;
        collider = &slot->display.healthCollider;
        mask = -5;
        mask &= collider->flags;
        collider->flags = mask;
        ContactSwarm_SetDirectionalSequence(pool, slot, 0);
    }
    {
        u32 phase = slot->stateFrame & 15;
        index &= 15;
        position = &slot->display.animation.draw.sprite.position;
        if (phase == index) {
            struct ContactSwarmSprite *current;
            const struct ContactPosition *target = &pool->target;
            s32 dx = target->x;
            s32 dz;
            current = sprite;
            dx -= current->position.x;
            dz = target->z;
            dz -= position->z;
            slot->direction = Math_Atan2(dx, dz);
            ContactSwarm_SetDirectionalSequence(pool, slot, 0);
        }
    }
    Collider_SetPosition(&slot->display.healthCollider, position, 0);
    {
        u32 frame = slot->stateFrame;
        register u32 limit asm("r5");
        limit = slot->trackingFrames;
        if (frame >= limit) {
            u32 offset = (u8 *)&pool->target - (u8 *)pool;
            const struct ContactPosition *target;
            struct ContactSwarmSprite *current;
            s32 dx;
            s32 dz;
            const u16 *randomTable;
            u32 randomOffset;
            u32 direction;
            u32 randomIndex;
            u32 jitter;
            register u32 mask asm("r2");
            target = (void *)((u8 *)pool + offset);
            dx = target->x;
            asm("" : "+r"(dx), "+r"(limit));
            current = sprite;
            dx -= current->position.x;
            dz = target->z;
            dz -= position->z;
            direction = Math_Atan2(dx, dz);
            randomIndex = (gRandomIndex + 1) & 1023;
            gRandomIndex = randomIndex;
            randomOffset = randomIndex * 2;
            randomTable = (const u16 *)RANDOM_TABLE_ADDR;
            randomOffset += (u32)randomTable;
            jitter = *(const u16 *)randomOffset;
            mask = 15;
            jitter &= mask;
            {
                u32 zero;
                u32 newDirection = direction + jitter - 8;
                zero = 0;
                slot->direction = newDirection;
                slot->state = 3;
                slot->enter = 1;
                slot->stateFrame = zero;
            }
        } else
            slot->stateFrame = frame + 1;
    }
}
