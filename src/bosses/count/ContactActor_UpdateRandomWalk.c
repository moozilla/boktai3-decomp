#include "scene/collision/contact_actor.h"
#include "engine/math/monster_particle_random.h"
#include "scene/motion/scene_actor_charge_offset.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
void ContactActor_UpdateRandomWalk(struct ContactActorPool *pool, struct ContactActor *slot,
                                   s32 index)
{
    struct ContactPosition delta;
    if (slot->enter) {
        slot->enter = 0;
        slot->direction = ParticleRandom_ReadDirectSample(gRandomTable);
        slot->steeringTimer = (ParticleRandom_ReadDirectSample(gRandomTable) & 15) + 16;
        ContactCollider_ClearFlags(&slot->healthCollider, 4);
    }
    if (pool->clearRequested) {
        ContactActor_EnterState(slot, 6);
        return;
    }
    if (!slot->steeringTimer) {
        slot->direction = ParticleRandom_ReadDirectSample(gRandomTable);
        slot->steeringTimer = (ParticleRandom_ReadDirectSample(gRandomTable) & 15) + 16;
    } else
        slot->steeringTimer--;
    {
        struct ContactPosition *write = &delta;
        u32 direction = slot->direction;
        u32 speed = slot->speed;
        write->x = SceneActor_ChargeComponent(gUnk_086149C4[(direction + 64) & 255] * speed);
        write->y = 0;
        write->z = SceneActor_ChargeComponent(gUnk_086149C4[direction] * speed);
    }
    {
        struct ContactPosition *position = &slot->position;
        const struct ContactPosition *read = &delta;
        position->x += read->x;
        position->y += read->y;
        position->z += read->z;
        Collider_SetPosition(&slot->healthCollider, position, 0);
    }
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (slot->stateFrame > 31) {
        ContactActor_EnterState(slot, 2);
    } else {
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
}
