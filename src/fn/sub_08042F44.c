#ifdef NONMATCHING
// fuzzy: 87.6%
#include "contact_actor.h"
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
static inline u32 nextSample(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
static inline void addPosition(struct ContactPosition *out, const struct ContactPosition *delta,
                               const struct ContactPosition *base)
{
    out->x = base->x + delta->x;
    out->y = base->y + delta->y;
    out->z = base->z + delta->z;
}
void ContactActor_UpdateRoaming(struct ContactActorPool *pool, struct ContactActor *slot, s32 index)
{
    struct ContactPosition delta;
    struct ContactPosition *destination;
    struct ContactPosition *position;
    if (slot->enter) {
        slot->enter = 0;
        if (slot->targetIndex >= 0) {
            u32 scale = 256;
            u32 angle = nextRandom();
            u32 mask = 255;
            struct ContactPosition *write = &delta;
            angle &= mask;
            write->x = truncateShift12(gUnk_086149C4[(angle + 64) & mask] * scale);
            write->y = 0;
            write->z = truncateShift12(gUnk_086149C4[angle] * scale);
            {
                struct ContactPosition *out = &slot->destination;
                addPosition(out, &delta, pool->targets[slot->targetIndex]);
                destination = out;
            }
        } else
            destination = &slot->destination;
        slot->direction =
            Math_Atan2(slot->destination.x - slot->position.x, destination->z - slot->position.z);
        {
            const u16 *table = gRandomTable;
            s32 jitter = (nextSample(table) & 63) - 32;
            slot->direction += jitter;
            slot->steeringTimer = (nextSample(table) & 15) + 16;
        }
        {
            struct ContactCollider *collider = &slot->healthCollider;
            s32 mask = -5;
            mask &= collider->flags;
            collider->flags = mask;
        }
    }
    if (pool->clearRequested) {
        {
            u32 state = 6;
            u32 frame = 0;
            slot->state = state;
            slot->enter = 1;
            slot->stateFrame = frame;
        }
        return;
    }
    if (!slot->steeringTimer) {
        const u16 *table = gRandomTable;
        u32 mask = 255;
        if ((nextSample(table) & mask) <= 191) {
            if (slot->targetIndex >= 0) {
                u32 scale = 256;
                u32 angle = nextSample(table);
                struct ContactPosition *write = &delta;
                angle &= mask;
                write->x = truncateShift12(gUnk_086149C4[(angle + 64) & mask] * scale);
                write->y = 0;
                write->z = truncateShift12(gUnk_086149C4[angle] * scale);
                {
                    struct ContactPosition *out = &slot->destination;
                    addPosition(out, &delta, pool->targets[slot->targetIndex]);
                    destination = out;
                }
            } else
                destination = &slot->destination;
            position = &slot->position;
            slot->direction =
                Math_Atan2(slot->destination.x - slot->position.x, destination->z - position->z);
            {
                s32 jitter = (nextRandom() & 63) - 32;
                slot->direction += jitter;
            }
        } else {
            slot->direction = nextSample(table);
            destination = &slot->destination;
            position = &slot->position;
        }
        slot->steeringTimer = (nextRandom() & 15) + 16;
    } else {
        slot->steeringTimer--;
        destination = &slot->destination;
        position = &slot->position;
    }
    if (slot->targetHeight < slot->desiredHeight)
        slot->targetHeight++;
    else if (slot->targetHeight > slot->desiredHeight)
        slot->targetHeight--;
    {
        struct ContactPosition *write = &delta;
        u32 direction = slot->direction;
        u32 speed = slot->speed;
        write->x = truncateShift12(gUnk_086149C4[(direction + 64) & 255] * speed);
        write->y = 0;
        write->z = truncateShift12(gUnk_086149C4[direction] * speed);
    }
    {
        const struct ContactPosition *read = &delta;
        const struct ContactPosition *first = &delta;
        slot->position.x += first->x;
        position->y += read->y;
        position->z += read->z;
    }
    Collider_SetPosition(&slot->healthCollider, position, 0);
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    {
        s32 dx = slot->destination.x - slot->position.x;
        s32 dz = destination->z - position->z;
        if (((dx * dx) + (dz * dz)) <= 16383) {
            slot->state = 2;
            slot->enter = 1;
            slot->stateFrame = 0;
        } else {
            if (slot->retargetTimer && (!(--slot->retargetTimer))) {
                if (pool->mode == 2)
                    slot->targetIndex = (index > 7) ? (0) : (1);
                else if (pool->mode == 1)
                    slot->targetIndex = 0;
                else
                    slot->targetIndex = 255;
            }
            slot->stateFrame++;
        }
    }
}

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", ContactActor_UpdateRoaming);
#endif
