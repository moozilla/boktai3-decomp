#include "scene/actor/contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
static inline s32 truncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline u32 nextRandom(const u16 *table)
{
    u32 index;
    index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
void ContactActor_UpdateRandomWalk(struct ContactActorPool *pool, struct ContactActor *slot,
                                   s32 index)
{
    struct ContactPosition delta;
    if (slot->enter) {
        const u16 *table;
        slot->enter = 0;
        table = (const u16 *)RANDOM_TABLE_ADDR;
        slot->direction = nextRandom(table);
        slot->steeringTimer = (nextRandom(table) & 15) + 16;
        {
            struct ContactCollider *collider = &slot->healthCollider;
            s32 mask = -5;
            mask &= collider->flags;
            collider->flags = mask;
        }
    }
    if (pool->clearRequested) {
        ContactActor_EnterState(slot, 6);
        return;
    }
    if (!slot->steeringTimer) {
        const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
        slot->direction = nextRandom(table);
        slot->steeringTimer = (nextRandom(table) & 15) + 16;
    } else
        slot->steeringTimer--;
    {
        struct ContactPosition *write = &delta;
        u32 direction = slot->direction;
        u32 speed = slot->speed;
        write->x = truncateShift12(gUnk_086149C4[(direction + 64) & 255] * speed);
        write->y = 0;
        write->z = truncateShift12(gUnk_086149C4[direction] * speed);
    }
    {
        struct ContactPosition *position = &slot->position;
        const struct ContactPosition *read = &delta;
        const struct ContactPosition *first = &delta;
        slot->position.x += first->x;
        position->y += read->y;
        position->z += read->z;
        Collider_SetPosition(&slot->healthCollider, position, 0);
    }
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (slot->stateFrame > 31) {
        ContactActor_EnterState(slot, 2);
    } else {
        if (slot->retargetTimer && !--slot->retargetTimer) {
            u32 mode = pool->mode;
            if (mode == 2) {
                if (index > 7)
                    slot->targetIndex = 0;
                else
                    slot->targetIndex = 1;
            } else if (mode == 1)
                slot->targetIndex = 0;
            else {
                mode = 255;
                slot->targetIndex = mode;
            }
        }
        slot->stateFrame++;
    }
}
