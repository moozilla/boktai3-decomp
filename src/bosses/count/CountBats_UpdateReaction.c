#include "scene/collision/contact_swarm.h"
#include "scene/motion/scene_actor_charge_offset.h"
void CountBats_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
u32 CountBats_HideSlot(struct ContactSwarm *, struct ContactSwarmSlot *, s32);
void CountBats_UpdateReaction(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    struct ContactSwarmSprite *sprite = &slot->display.animation.draw.sprite;
    struct ContactPosition *velocity;
    const struct ContactPosition *read;
    s32 product;
    u32 rawSpeed;
    u32 direction;
    u32 speed;
    if (slot->enter) {
        slot->enter = 0;
        ContactCollider_SetFlags(&slot->display.healthCollider, 4);
        AnimData_SelectPaletteBank(&slot->display.animation.draw.graphics, 0x192);
        CountBats_SetDirectionalSequence(pool, slot, 1);
        Sound_PlaySong(0xE8);
    }
    rawSpeed = 20 - slot->stateFrame;
    velocity = &slot->velocity;
    direction = slot->direction;
    speed = rawSpeed * 2;
    product = gUnk_086149C4[(direction + 64) & 255] * speed;
    read = velocity;
    velocity->x = SceneActor_ChargeComponent(product);
    velocity->y = 0;
    velocity->z = SceneActor_ChargeComponent(gUnk_086149C4[direction] * speed);
    {
        struct ContactPosition *position = &sprite->position;
        sprite->position.x += slot->velocity.x;
        position->y += read->y;
        position->z += read->z;
    }
    if (slot->stateFrame > 19)
        CountBats_HideSlot(pool, slot, index);
    else {
        if (slot->stateFrame == 6)
            AnimData_SelectPaletteBank(&slot->display.animation.draw.graphics, 0x2D3);
        if ((slot->stateFrame >> 2) & 1)
            slot->display.animation.draw.sprite.flags &= ~1;
        else
            slot->display.animation.draw.sprite.flags |= 1;
        slot->stateFrame++;
    }
}
