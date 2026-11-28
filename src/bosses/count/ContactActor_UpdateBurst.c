#include "scene/collision/contact_actor.h"
#include "scene/motion/scene_actor_charge_offset.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
void ContactActor_UpdateBurst(struct ContactActorPool *pool, struct ContactActor *slot, s32 index)
{
    struct ContactPosition delta;
    u32 speed;
    if (slot->enter) {
        struct ContactPosition *position;
        slot->enter = 0;
        ContactCollider_ClearFlags(&slot->healthCollider, 4);
        position = &slot->position;
        slot->origin = slot->position;
        if (slot->targetIndex >= 0) {
            const struct ContactPosition *target = pool->targets[slot->targetIndex];
            slot->direction = Math_Atan2(target->x - slot->position.x, target->z - position->z);
        }
    }
    if (pool->clearRequested) {
        ContactActor_EnterState(slot, 6);
        return;
    }
    if (slot->stateFrame <= 15)
        speed = slot->stateFrame * 16;
    else
        speed = (32 - slot->stateFrame) * 16;
    {
        struct ContactPosition *write = &delta;
        u32 direction = slot->direction;
        write->x = SceneActor_ChargeComponent(gUnk_086149C4[(direction + 64) & 255] * speed);
        write->y = 0;
        write->z = SceneActor_ChargeComponent(gUnk_086149C4[direction] * speed);
    }
    {
        struct ContactPosition *position = &slot->position;
        const struct ContactPosition *origin = &slot->origin;
        const struct ContactPosition *read = &delta;
        position->x = origin->x + read->x;
        position->y = origin->y + read->y;
        position->z = origin->z + read->z;
        Collider_SetPosition(&slot->resourceCollider, position, 0);
        Collider_SetPosition(&slot->healthCollider, position, 0);
        Collider_Refresh(&slot->resourceCollider);
    }
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (slot->stateFrame > 31) {
        ContactActor_EnterState(slot, 2);
    } else {
        if (slot->retargetTimer) {
            slot->retargetTimer--;
            if (!slot->retargetTimer) {
                if (pool->mode == 2) {
                    if (index <= 7)
                        slot->targetIndex = 1;
                    else
                        slot->targetIndex = 0;
                } else if (pool->mode == 1)
                    slot->targetIndex = 0;
                else
                    slot->targetIndex = -1;
            }
        }
        slot->stateFrame++;
    }
}
