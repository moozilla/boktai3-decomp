#include "scene/collision/contact_actor.h"
#include "scene/actor/scene_actor_part_burst.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
static inline s32 truncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline u32 nextRandom(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return ((const u16 *)RANDOM_TABLE_ADDR)[index];
}
static inline u32 headingTo(const struct ContactPosition *to, const struct ContactPosition *from)
{
    return Math_Atan2(to->x - from->x, to->z - from->z);
}
static inline void movePosition(struct ContactPosition *p, const struct ContactPosition *d)
{
    p->x += d->x;
    p->y += d->y;
    p->z += d->z;
}
static inline s32 distanceSq(const struct ContactPosition *a, const struct ContactPosition *b)
{
    s32 dx = a->x - b->x;
    s32 dz = a->z - b->z;
    return dx * dx + dz * dz;
}
static inline void polarOffset(struct ContactPosition *write, u32 angle, u32 radius)
{
    write->x = truncateShift12(gUnk_086149C4[(angle + 64) & 255] * radius);
    write->y = 0;
    write->z = truncateShift12(gUnk_086149C4[angle] * radius);
}
static inline void addPosition(struct ContactPosition *out, const struct ContactPosition *base,
                               const struct ContactPosition *delta)
{
    out->x = base->x + delta->x;
    out->y = base->y + delta->y;
    out->z = base->z + delta->z;
}
void ContactActor_UpdateRoaming(struct ContactActorPool *pool, struct ContactActor *slot, s32 index)
{
    struct ContactPosition delta;
    /* Scratch pointer: the health collider on entry, the random table while steering. */
    void *work;

    if (slot->enter) {
        const u16 *table;
        s32 jitter;
        slot->enter = 0;
        if (slot->targetIndex >= 0) {
            polarOffset(&delta, nextRandom() & 255, 256);
            addPosition(&slot->destination, pool->targets[slot->targetIndex], &delta);
        }
        slot->direction = headingTo(&slot->destination, &slot->position);
        table = gRandomTable;
        jitter = (SceneActorEffects_NextSample(table) & 63) - 32;
        slot->direction += jitter;
        slot->steeringTimer = (SceneActorEffects_NextSample(table) & 15) + 16;
        work = &slot->healthCollider;
        ContactCollider_ClearFlags(work, 4);
    }
    if (pool->clearRequested) {
        ContactActor_EnterState(slot, 6);
        return;
    }
    if (!slot->steeringTimer) {
        s32 roll;
        work = (void *)gRandomTable;
        roll = SceneActorEffects_NextSample(work) & 255;
        if (roll <= 191) {
            s32 jitter;
            if (slot->targetIndex >= 0) {
                polarOffset(&delta, SceneActorEffects_NextSample(work) & 255, 256);
                addPosition(&slot->destination, pool->targets[slot->targetIndex], &delta);
            }
            slot->direction = headingTo(&slot->destination, &slot->position);
            jitter = (nextRandom() & 63) - 32;
            slot->direction += jitter;
        } else
            slot->direction = SceneActorEffects_NextSample(work);
        slot->steeringTimer = (nextRandom() & 15) + 16;
    } else
        slot->steeringTimer--;
    if (slot->targetHeight < slot->desiredHeight)
        slot->targetHeight++;
    else if (slot->targetHeight > slot->desiredHeight)
        slot->targetHeight--;
    polarOffset(&delta, slot->direction, slot->speed);
    movePosition(&slot->position, &delta);
    Collider_SetPosition(&slot->healthCollider, &slot->position, 0);
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (distanceSq(&slot->destination, &slot->position) <= 16383) {
        ContactActor_EnterState(slot, 2);
    } else {
        if (slot->retargetTimer) {
            slot->retargetTimer--;
            if (slot->retargetTimer == 0) {
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
