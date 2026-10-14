// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#define ContactSwarm_UpdateReaction         sub_08044480
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
void ContactSwarm_UpdateReaction(struct ContactSwarm *pool, struct ContactSwarmSlot *slot,
                                 s32 index)
{
    struct ContactSwarmSprite *sprite = &slot->sprite;
    struct ContactPosition *velocity;
    register const struct ContactPosition *read asm("r12");
    s32 product;
    register u32 rawSpeed asm("r0");
    u32 direction;
    u32 speed;
    if (slot->enter) {
        struct ContactCollider *collider;
        slot->enter = 0;
        collider = &slot->healthCollider;
        collider->flags |= 4;
        AnimData_SelectTiles(slot->graphics, 0x192);
        ContactSwarm_SetDirectionalSequence(pool, slot, 1);
        Sound_PlaySong(0xE8);
    }
    rawSpeed = 20 - slot->stateFrame;
    asm("" : "+r"(rawSpeed));
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
            register const struct ContactPosition *current asm("r2") = read;
            position->y += current->y;
        }
        {
            register const struct ContactPosition *current asm("r2") = read;
            position->z += current->z;
        }
    }
    if (slot->stateFrame > 19)
        ContactSwarm_HideSlot(pool, slot, index);
    else {
        if (slot->stateFrame == 6)
            AnimData_SelectTiles(slot->graphics, 0x2D3);
        if ((slot->stateFrame >> 2) & 1)
            slot->sprite.flags &= ~1;
        else
            slot->sprite.flags |= 1;
        slot->stateFrame++;
    }
}
