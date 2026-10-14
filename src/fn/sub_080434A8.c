// MATCHING_CONCESSION: register-pinning
#include "contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
static inline s32 truncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void ContactActor_UpdateBurst(struct ContactActorPool *pool, struct ContactActor *slot, s32 index)
{
    struct ContactPosition delta;
    u32 zero;
    u32 frame;
    if (slot->enter) {
        struct ContactPosition *position;
        slot->enter = 0;
        {
            struct ContactCollider *collider = &slot->healthCollider;
            s32 mask = -5;
            mask &= collider->flags;
            collider->flags = mask;
        }
        position = &slot->position;
        slot->origin = slot->position;
        if (slot->targetIndex >= 0) {
            const struct ContactPosition *target = pool->targets[slot->targetIndex];
            slot->direction = Math_Atan2(target->x - slot->position.x, target->z - position->z);
        }
    }
    if (pool->clearRequested) {
        u32 state = 6;
        u32 zeroFrame = 0;
        slot->state = state;
        slot->enter = 1;
        slot->stateFrame = zeroFrame;
        return;
    }
    frame = slot->stateFrame;
    asm("" : "+r"(frame) : : "memory");
    if (frame > 15)
        frame = 32 - slot->stateFrame;
    {
        u32 speed = frame * 16;
        register struct ContactPosition *write asm("r4") = &delta;
        register u32 direction asm("r5") = slot->direction;
        write->x = truncateShift12(gUnk_086149C4[(direction + 64) & 255] * speed);
        write->y = 0;
        {
            register const s16 *entry asm("r0") = gUnk_086149C4 + direction;
            {
                s32 z = truncateShift12(*entry * speed);
                register u32 lowZero asm("r3") = 0;
                asm("" : "+r"(lowZero));
                zero = lowZero;
                write->z = z;
            }
        }
    }
    {
        struct ContactPosition *position = &slot->position;
        register const struct ContactPosition *origin asm("r2") = &slot->origin;
        register const struct ContactPosition *read asm("r1") = &delta;
        const struct ContactPosition *first = &delta;
        struct ContactCollider *resource;
        asm("" : "+r"(origin));
        {
            register u32 x asm("r0") = (u16)first->x;
            asm("" : "+r"(x));
            slot->position.x = x + (u16)slot->origin.x;
        }
        position->y = read->y + origin->y;
        {
            register u32 z asm("r0") = (u16)read->z;
            {
                register u32 baseZ asm("r2") = (u16)origin->z;
                position->z = z + baseZ;
            }
        }
        resource = &slot->resourceCollider;
        Collider_SetPosition(resource, position, 0);
        Collider_SetPosition(&slot->healthCollider, position, 0);
        Collider_Refresh(resource);
    }
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (slot->stateFrame > 31) {
        slot->state = 2;
        slot->enter = 1;
        slot->stateFrame = 0;
    } else {
        if (slot->retargetTimer && !--slot->retargetTimer) {
            register u32 mode asm("r0") = pool->mode;
            if (mode == 2) {
                register s32 slotIndex asm("r4") = index;
                if (slotIndex > 7)
                    goto zeroMode2;
                mode = 1;
                goto storeTarget;
            zeroMode2:
                mode = zero;
                goto storeTarget;
            }
            if (mode != 1)
                goto minusTarget;
            {
                register u32 value asm("r1") = zero;
                slot->targetIndex = value;
                goto incrementFrame;
            }
        minusTarget:
            mode = 255;
        storeTarget:
            slot->targetIndex = mode;
        }
    incrementFrame:
        slot->stateFrame++;
    }
}
