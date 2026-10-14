// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#define ContactSwarm_UpdateCharge           sub_0804437C
#define ContactSwarm_SetDirectionalSequence sub_08044174
#define ContactSwarm_HideSlot               sub_0804413C
void ContactSwarm_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
u32 ContactSwarm_HideSlot(struct ContactSwarm *, struct ContactSwarmSlot *, s32);
static inline s32 truncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void ContactSwarm_UpdateCharge(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    struct ContactSwarmSprite *sprite = &slot->display.animation.draw.sprite;
    struct ContactPosition *position;
    if (slot->enter) {
        struct ContactCollider *collider;
        struct ContactPosition *velocity;
        register u32 direction asm("r4");
        u32 speed;
        s32 mask;
        slot->enter = 0;
        collider = &slot->display.healthCollider;
        mask = -5;
        mask &= collider->flags;
        collider->flags = mask;
        ContactSwarm_SetDirectionalSequence(pool, slot, 0);
        velocity = &slot->velocity;
        direction = slot->direction;
        speed = slot->speed;
        velocity->x = truncateShift12(gUnk_086149C4[(direction + 64) & 255] * speed);
        velocity->y = 0;
        {
            const s16 *table = gUnk_086149C4;
            register u32 offset asm("r0");
            const s16 *entry;
            offset = direction * 2;
            entry = (const void *)(offset + (u32)table);
            velocity->z = truncateShift12(*entry * speed);
        }
    }
    position = &sprite->position;
    {
        const struct ContactPosition *velocity = &slot->velocity;
        sprite->position.x += slot->velocity.x;
        position->y += velocity->y;
        position->z += velocity->z;
    }
    Collider_SetPosition(&slot->display.healthCollider, position, 0);
    if ((index & 1) == pool->resourceParity) {
        struct ContactCollider *resource = &slot->resourceCollider;
        Collider_SetPosition(resource, position, 0);
        {
            s32 count = resource->unknown3C;
            if (count > 0)
                Collider_Refresh(resource);
        }
    }
    if (slot->stateFrame > 119)
        ContactSwarm_HideSlot(pool, slot, index);
    else
        slot->stateFrame++;
}
