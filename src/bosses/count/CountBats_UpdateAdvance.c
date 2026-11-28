#include "scene/collision/contact_swarm.h"
#include "scene/motion/scene_actor_charge_offset.h"
void CountBats_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
void CountBats_UpdateAdvance(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    struct ContactSwarmSprite *sprite = &slot->display.animation.draw.sprite;
    if (slot->enter) {
        struct ContactPosition *velocity;
        u32 direction;
        u32 speed;
        slot->enter = 0;
        ContactCollider_SetFlags(&slot->display.healthCollider, 4);
        CountBats_SetDirectionalSequence(pool, slot, 0);
        velocity = &slot->velocity;
        direction = slot->direction;
        speed = slot->advanceSpeed;
        velocity->x = SceneActor_ChargeComponent(gUnk_086149C4[(direction + 64) & 255] * speed);
        velocity->y = 0;
        velocity->z = SceneActor_ChargeComponent(gUnk_086149C4[direction] * speed);
    }
    {
        struct ContactPosition *position = &sprite->position;
        const struct ContactPosition *velocity = &slot->velocity;
        position->x += velocity->x;
        position->y += velocity->y;
        position->z += velocity->z;
    }
    if (slot->stateFrame >= slot->advanceFrames) {
        slot->state = 2;
        slot->enter = 1;
        slot->stateFrame = 0;
    } else
        slot->stateFrame++;
}
