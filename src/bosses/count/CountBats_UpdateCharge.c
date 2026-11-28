#include "scene/collision/contact_swarm.h"
#include "scene/motion/scene_actor_charge_offset.h"
void CountBats_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
u32 CountBats_HideSlot(struct ContactSwarm *, struct ContactSwarmSlot *, s32);
void CountBats_UpdateCharge(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    struct ContactSwarmSprite *sprite = &slot->display.animation.draw.sprite;
    s32 k2 = 255;
    struct ContactPosition *position;
    if (slot->enter) {
        struct ContactPosition *velocity;
        u32 direction;
        u32 speed;
        slot->enter = 0;
        ContactCollider_ClearFlags(&slot->display.healthCollider, 4);
        CountBats_SetDirectionalSequence(pool, slot, 0);
        velocity = &slot->velocity;
        direction = slot->direction;
        speed = slot->speed;
        velocity->x = SceneActor_ChargeComponent(gUnk_086149C4[(direction + 64) & k2] * speed);
        velocity->y = 0;
        velocity->z = SceneActor_ChargeComponent(gUnk_086149C4[direction] * speed);
    }
    position = &sprite->position;
    {
        const struct ContactPosition *velocity = &slot->velocity;
        position->x += velocity->x;
        position->y += velocity->y;
        position->z += velocity->z;
    }
    Collider_SetPosition(&slot->display.healthCollider, position, 0);
    if ((index & 1) == pool->resourceParity) {
        struct ContactCollider *resource = &slot->resourceCollider;
        Collider_SetPosition(resource, position, 0);
        {
            s32 count = resource->unknown3C;
            if (count > 0) {
                Collider_Refresh(resource);
            }
        }
    }
    if (slot->stateFrame > 119)
        CountBats_HideSlot(pool, slot, index);
    else
        slot->stateFrame++;
}
