#include "contact_swarm.h"
#define ContactSwarm_UpdateAdvance          sub_080441D0
#define ContactSwarm_SetDirectionalSequence sub_08044174
void ContactSwarm_SetDirectionalSequence(struct ContactSwarm *, struct ContactSwarmSlot *, u32);
static inline s32 truncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void ContactSwarm_UpdateAdvance(struct ContactSwarm *pool, struct ContactSwarmSlot *slot, s32 index)
{
    struct ContactSwarmSprite *sprite = &slot->animation.sprite;
    if (slot->enter) {
        struct ContactCollider *collider;
        struct ContactPosition *velocity;
        u32 direction;
        u32 speed;
        slot->enter = 0;
        collider = &slot->healthCollider;
        collider->flags |= 4;
        ContactSwarm_SetDirectionalSequence(pool, slot, 0);
        velocity = &slot->velocity;
        direction = slot->direction;
        speed = slot->advanceSpeed;
        velocity->x = truncateShift12(gUnk_086149C4[(direction + 64) & 255] * speed);
        velocity->y = 0;
        {
            const s16 *table = gUnk_086149C4;
            const s16 *entry;
            u32 offset;
            offset = direction * 2;
            entry = (const void *)(offset + (u32)table);
            velocity->z = truncateShift12(*entry * speed);
        }
    }
    {
        struct ContactPosition *position = &sprite->position;
        const struct ContactPosition *velocity = &slot->velocity;
        sprite->position.x += slot->velocity.x;
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
