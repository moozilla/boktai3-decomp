#include "scene/collision/contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
void ContactActor_UpdateTracking(struct ContactActorPool *pool, struct ContactActor *slot,
                                 s32 index)
{
    if (slot->enter) {
        slot->enter = 0;
        ContactCollider_ClearFlags(&slot->healthCollider, 4);
        Collider_SetPosition(&slot->healthCollider, &slot->position, 0);
    }
    if (pool->clearRequested) {
        ContactActor_EnterState(slot, 6);
        return;
    }
    if (slot->targetIndex >= 0) {
        u32 phase = slot->stateFrame & 7;
        if (!phase) {
            const struct ContactPosition *target;
            const struct ContactPosition *position;
            s32 dx, dz;
            target = pool->targets[slot->targetIndex];
            position = &slot->position;
            dx = target->x - position->x;
            dz = target->z - position->z;
            if (dx * dx + dz * dz > 0x22E24) {
                ContactActor_EnterState(slot, 1);
                return;
            }
        }
        {
            const struct ContactPosition *target = pool->targets[slot->targetIndex];
            const struct ContactPosition *position = &slot->position;
            slot->direction = Math_Atan2(target->x - position->x, target->z - position->z);
        }
    }
    Collider_SetPosition(&slot->healthCollider, &slot->position, 0);
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (slot->stateFrame > 63) {
        ContactActor_EnterState(slot, 4);
    }
    if (slot->retargetTimer) {
        slot->retargetTimer--;
        if (!slot->retargetTimer) {
            if (pool->mode == 2) {
                if (index > 7)
                    slot->targetIndex = 0;
                else
                    slot->targetIndex = 1;
            } else if (pool->mode == 1)
                slot->targetIndex = 0;
            else
                slot->targetIndex = -1;
        }
    }
    slot->stateFrame++;
}
