#include "scene/collision/contact_swarm.h"
void CountBats_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
void CountBats_UpdateTracking(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    struct ContactSwarmSprite *sprite = &slot->display.animation.draw.sprite;
    struct ContactPosition *position;
    u32 frame;
    if (slot->enter) {
        slot->enter = 0;
        ContactCollider_ClearFlags(&slot->display.healthCollider, 4);
        CountBats_SetDirectionalSequence(pool, slot, 0);
    }
    {
        u32 phase = slot->stateFrame & 15;
        index &= 15;
        position = &slot->display.animation.draw.sprite.position;
        if (phase == index) {
            const struct ContactPosition *target = &pool->target;
            slot->direction = Math_Atan2(target->x - sprite->position.x, target->z - position->z);
            CountBats_SetDirectionalSequence(pool, slot, 0);
            Collider_SetPosition(&slot->display.healthCollider, position, 0);
            frame = slot->stateFrame;
        } else {
            Collider_SetPosition(&slot->display.healthCollider, position, 0);
            frame = slot->stateFrame;
        }
    }
    {
        if (frame >= slot->trackingFrames) {
            const struct ContactPosition *target;
            s32 dx;
            s32 dz;
            u32 direction;
            u32 jitter;
            target = &pool->target;
            dx = target->x;
            dx -= sprite->position.x;
            dz = target->z;
            dz -= position->z;
            direction = Math_Atan2(dx, dz);
            gRandomIndex = (gRandomIndex + 1) & 1023;
            jitter = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
            jitter &= 15;
            slot->direction = direction + jitter - 8;
            slot->state = 3;
            slot->enter = 1;
            slot->stateFrame = 0;
        } else
            slot->stateFrame = frame + 1;
    }
}
