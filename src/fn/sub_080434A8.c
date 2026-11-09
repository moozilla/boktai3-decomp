#include "contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
static inline s32 truncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void ContactActor_UpdateBurst(struct ContactActorPool *pool, struct ContactActor *slot, s32 index)
{
    struct ContactPosition delta;
    u32 zero;
    u32 frame;
    u32 speed;
    if (slot->enter) {
        struct ContactPosition *position;
        slot->enter = 0;
        {
            struct ContactCollider *collider = &slot->healthCollider;
            s32 mask = -5;
            mask &= collider->flags;
            collider->flags = mask;
        }
        position = &slot->position;
        slot->origin = slot->position;
        if (slot->targetIndex >= 0) {
            const struct ContactPosition *target = pool->targets[slot->targetIndex];
            slot->direction = Math_Atan2(target->x - slot->position.x, target->z - position->z);
        }
    }
    if (pool->clearRequested) {
        u32 state = 6;
        u32 zeroFrame = 0;
        slot->state = state;
        slot->enter = 1;
        slot->stateFrame = zeroFrame;
        return;
    }
    if (slot->stateFrame <= 15)
        speed = slot->stateFrame * 16;
    else
        speed = (32 - slot->stateFrame) * 16;
    {
        struct ContactPosition *write = &delta;
        u32 direction = slot->direction;
        write->x = truncateShift12(gUnk_086149C4[(direction + 64) & 255] * speed);
        write->y = 0;
        write->z = truncateShift12(gUnk_086149C4[direction] * speed);
    }
    zero = 0;
    {
        struct ContactPosition *position = &slot->position;
        const struct ContactPosition *origin = &slot->origin;
        const struct ContactPosition *read = &delta;
        const struct ContactPosition *first = &delta;
        struct ContactCollider *resource;
        slot->position.x = slot->origin.x + first->x;
        position->y = origin->y + read->y;
        position->z = origin->z + read->z;
        resource = &slot->resourceCollider;
        Collider_SetPosition(resource, position, 0);
        Collider_SetPosition(&slot->healthCollider, position, 0);
        Collider_Refresh(resource);
    }
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (slot->stateFrame > 31) {
        slot->state = 2;
        slot->enter = 1;
        slot->stateFrame = 0;
    } else {
        if (slot->retargetTimer && !--slot->retargetTimer) {
            u32 mode = pool->mode;
            if (mode == 2) {
                if (index <= 7)
                    slot->targetIndex = 1;
                else
                    slot->targetIndex = zero;
            } else if (mode == 1)
                slot->targetIndex = zero;
            else {
                mode = 255;
                slot->targetIndex = mode;
            }
        }
        slot->stateFrame++;
    }
}
