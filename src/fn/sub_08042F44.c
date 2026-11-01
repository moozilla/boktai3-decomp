#ifdef NONMATCHING
// fuzzy: 97.4%
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
            addPosition(&slot->destination, pool->targets[slot->targetIndex], &delta);
        }
        slot->direction = headingTo(&slot->destination, &slot->position);
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
        s32 roll = nextSample(table) & mask;
        if (roll <= 191) {
            if (slot->targetIndex >= 0) {
                u32 scale = 256;
                u32 angle = nextSample(table);
                struct ContactPosition *write = &delta;
                angle &= mask;
                write->x = truncateShift12(gUnk_086149C4[(angle + 64) & mask] * scale);
                write->y = 0;
                write->z = truncateShift12(gUnk_086149C4[angle] * scale);
                addPosition(&slot->destination, pool->targets[slot->targetIndex], &delta);
            }
            slot->direction = headingTo(&slot->destination, &slot->position);
            {
                s32 jitter = (nextRandom() & 63) - 32;
                slot->direction += jitter;
            }
        } else
            slot->direction = nextSample(table);
        slot->steeringTimer = (nextRandom() & 15) + 16;
    } else
        slot->steeringTimer--;
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
    movePosition(&slot->position, &delta);
    Collider_SetPosition(&slot->healthCollider, &slot->position, 0);
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (distanceSq(&slot->destination, &slot->position) <= 16383) {
        slot->state = 2;
        slot->enter = 1;
        slot->stateFrame = 0;
    } else {
        if (slot->retargetTimer) {
            u32 timer = slot->retargetTimer - 1;
            slot->retargetTimer = timer;
            if (!(u8)timer) {
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

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", ContactActor_UpdateRoaming);
#endif
