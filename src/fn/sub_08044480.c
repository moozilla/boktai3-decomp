#include "contact_swarm.h"
void ContactSwarm_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
u32 ContactSwarm_HideSlot(struct ContactSwarm *, struct ContactSwarmSlot *, s32);
static inline s32 truncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void ContactSwarm_UpdateReaction(struct ContactSwarm *pool, struct ContactSwarmSlot *slot,
                                 s32 index)
{
    struct ContactSwarmSprite *sprite = &slot->display.animation.draw.sprite;
    struct ContactPosition *velocity;
    const struct ContactPosition *read;
    s32 product;
    u32 rawSpeed;
    u32 direction;
    u32 speed;
    if (slot->enter) {
        struct ContactCollider *collider;
        slot->enter = 0;
        collider = &slot->display.healthCollider;
        collider->flags |= 4;
        AnimData_SelectPaletteBank(&slot->display.animation.draw.graphics, 0x192);
        ContactSwarm_SetDirectionalSequence(pool, slot, 1);
        Sound_PlaySong(0xE8);
    }
    rawSpeed = 20 - slot->stateFrame;
    velocity = &slot->velocity;
    direction = slot->direction;
    speed = rawSpeed * 2;
    product = gUnk_086149C4[(direction + 64) & 255] * speed;
    read = velocity;
    velocity->x = truncateShift12(product);
    velocity->y = 0;
    {
        const s16 *table = gUnk_086149C4;
        u32 offset = direction * 2;
        const s16 *entry = (const void *)(offset + (u32)table);
        velocity->z = truncateShift12(*entry * speed);
    }
    {
        struct ContactPosition *position = &sprite->position;
        sprite->position.x += slot->velocity.x;
        {
            const struct ContactPosition *current = read;
            position->y += current->y;
        }
        {
            const struct ContactPosition *current = read;
            position->z += current->z;
        }
    }
    if (slot->stateFrame > 19)
        ContactSwarm_HideSlot(pool, slot, index);
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
