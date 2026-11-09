#include "contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
static inline struct ContactPosition *ContactActor_GetPosition(struct ContactActor *slot)
{
    return &slot->position;
}
void ContactActor_UpdateTracking(struct ContactActorPool *pool, struct ContactActor *slot,
                                 s32 index)
{
    if (slot->enter) {
        slot->enter = 0;
        {
            struct ContactCollider *collider = &slot->healthCollider;
            s32 mask = -5;
            mask &= collider->flags;
            collider->flags = mask;
            Collider_SetPosition(collider, &slot->position, 0);
        }
    }
    if (pool->clearRequested) {
        u32 state = 6;
        u32 frame = 0;
        slot->state = state;
        slot->enter = 1;
        slot->stateFrame = frame;
        return;
    }
    if (slot->targetIndex >= 0) {
        u32 phase = slot->stateFrame & 7;
        if (!phase) {
            const struct ContactPosition *target;
            s32 dx, dz;
            target = pool->targets[slot->targetIndex];
            dx = target->x - slot->position.x;
            dz = target->z - ContactActor_GetPosition(slot)->z;
            if (dx * dx + dz * dz > 0x22E24) {
                slot->state = 1;
                slot->enter = 1;
                slot->stateFrame = phase;
                return;
            }
        }
        {
            const struct ContactPosition *target = pool->targets[slot->targetIndex];
            slot->direction = Math_Atan2(target->x - slot->position.x,
                                         target->z - ContactActor_GetPosition(slot)->z);
        }
    }
    Collider_SetPosition(&slot->healthCollider, &slot->position, 0);
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (slot->stateFrame > 63) {
        u32 state = 4;
        u32 frame = 0;
        slot->state = state;
        slot->enter = 1;
        slot->stateFrame = frame;
    }
    if (slot->retargetTimer) {
        u32 remaining = (u8)--slot->retargetTimer;
        if (!remaining) {
            u32 mode = pool->mode;
            if (mode == 2) {
                if (index > 7)
                    slot->targetIndex = remaining;
                else
                    slot->targetIndex = 1;
            } else if (mode == 1)
                slot->targetIndex = remaining;
            else {
                mode = 255;
                slot->targetIndex = mode;
            }
        }
    }
    slot->stateFrame++;
}
