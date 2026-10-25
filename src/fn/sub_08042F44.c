#ifdef NONMATCHING
// fuzzy: 65.8%
// fix.py best (sweep after the inlined-insn fix)
// Reviewed full extent; register lifetimes and instruction scheduling still differ.
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
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    return ((const u16 *)RANDOM_TABLE_ADDR)[index];
}
void ContactActor_UpdateRoaming(struct ContactActorPool *pool, struct ContactActor *slot, s32 index)
{
    struct ContactPosition delta;
    struct ContactPosition *destination;
    struct ContactPosition *position;
    if (slot->enter) {
        slot->enter = 0;
        destination = &slot->destination;
        if (slot->targetIndex >= 0) {
            u32 angle = nextRandom() & 255;
            const struct ContactPosition *target;
            struct ContactPosition *write = &delta;
            write->x = truncateShift12(gUnk_086149C4[(angle + 64) & 255] * 256);
            write->y = 0;
            write->z = truncateShift12(gUnk_086149C4[angle] * 256);
            target = pool->targets[slot->targetIndex];
            {
                const struct ContactPosition *read = &delta;
                const struct ContactPosition *first = &delta;
                slot->destination.x = first->x + target->x;
                destination->y = read->y + target->y;
                destination->z = read->z + target->z;
            }
        }
        slot->direction =
            Math_Atan2(slot->destination.x - slot->position.x, destination->z - slot->position.z);
        slot->direction += (nextRandom() & 63) - 32;
        slot->steeringTimer = (nextRandom() & 15) + 16;
        slot->healthCollider.flags &= ~4;
    }
    if (pool->clearRequested) {
        slot->state = 6;
        slot->enter = 1;
        slot->stateFrame = 0;
        return;
    }
    if (!slot->steeringTimer) {
        if ((nextRandom() & 255) <= 191) {
            destination = &slot->destination;
            if (slot->targetIndex >= 0) {
                u32 angle = nextRandom() & 255;
                const struct ContactPosition *target;
                struct ContactPosition *write = &delta;
                write->x = truncateShift12(gUnk_086149C4[(angle + 64) & 255] * 256);
                write->y = 0;
                write->z = truncateShift12(gUnk_086149C4[angle] * 256);
                target = pool->targets[slot->targetIndex];
                {
                    const struct ContactPosition *read = &delta;
                    const struct ContactPosition *first = &delta;
                    slot->destination.x = first->x + target->x;
                    destination->y = read->y + target->y;
                    destination->z = read->z + target->z;
                }
            }
            position = &slot->position;
            slot->direction =
                Math_Atan2(slot->destination.x - slot->position.x, destination->z - position->z);
            slot->direction += (nextRandom() & 63) - 32;
        } else {
            slot->direction = nextRandom();
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
        {
            typeof(write->x) *q1 = &write->x;
            u8 z1 = 0;
            *q1 = truncateShift12(gUnk_086149C4[(direction + 64) & 255] * speed);
            write->y = z1;
            write->z = truncateShift12(gUnk_086149C4[direction] * speed);
        }
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
