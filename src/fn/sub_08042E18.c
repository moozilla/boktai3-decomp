// MATCHING_CONCESSION: register-pinning
#include "contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
void ContactActor_UpdateTracking(struct ContactActorPool *poolInput, struct ContactActor *slotInput,
                                 s32 indexInput)
{
    register struct ContactActorPool *pool asm("r8") = poolInput;
    s32 index;
    register struct ContactActor *slot asm("r4");
    struct ContactPosition *position;
    s32 targetIndex;
    slot = slotInput;
    index = indexInput;
    if (slot->enter) {
        slot->enter = 0;
        {
            struct ContactCollider *collider = &slot->healthCollider;
            s32 mask = -5;
            mask &= collider->flags;
            collider->flags = mask;
            Collider_SetPosition(collider, &slot->position, 0);
        }
    }
    {
        register const struct ContactActorPool *current asm("r1") = pool;
        if (current->clearRequested) {
            {
                u32 state = 6;
                u32 zero;
                zero = 0;
                slot->state = state;
                slot->enter = 1;
                slot->stateFrame = zero;
                goto done;
            }
        }
    }
    targetIndex = slot->targetIndex;
    position = &slot->position;
    asm("" : "+r"(position), "+r"(targetIndex) : : "memory");
    if (targetIndex >= 0) {
        const struct ContactPosition *target;
        s32 dx, dz;
        u32 frame = slot->stateFrame;
        u32 phase;
        register s32 secondIndex asm("r2");
        register s32 savedIndex asm("r12");
        register const struct ContactPosition **targets asm("r6");
        phase = 7;
        phase &= frame;
        secondIndex = slot->targetIndex;
        savedIndex = secondIndex;
        targets = pool->targets;
        if (!phase) {
            register s32 distanceX asm("r2");
            s32 distanceZ;
            {
                register const struct ContactPosition **address asm("r0") = &targets[secondIndex];
                target = *address;
                distanceX = target->x;
                asm("" : "+r"(address), "+r"(distanceX));
            }
            distanceX -= slot->position.x;
            distanceZ = target->z;
            distanceZ -= position->z;
            if (distanceX * distanceX + distanceZ * distanceZ > 0x22E24) {
                slot->state = 1;
                slot->enter = 1;
                slot->stateFrame = phase;
                goto done;
            }
        }
        {
            register const struct ContactPosition **address asm("r0") = &targets[savedIndex];
            const struct ContactPosition *secondTarget;
            secondTarget = *address;
            dx = secondTarget->x - slot->position.x;
            dz = secondTarget->z - position->z;
            slot->direction = Math_Atan2(dx, dz);
        }
    }
    Collider_SetPosition(&slot->healthCollider, position, 0);
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (slot->stateFrame > 63) {
        u32 state = 4;
        u32 zero;
        zero = 0;
        slot->state = state;
        slot->enter = 1;
        slot->stateFrame = zero;
    }
    {
        register u32 timer asm("r0") = slot->retargetTimer;
        if (timer) {
            register u32 remaining asm("r1");
            register u32 mode asm("r0");
            timer--;
            slot->retargetTimer = timer;
            timer <<= 24;
            remaining = timer >> 24;
            if (!remaining) {
                {
                    const struct ContactActorPool *current = pool;
                    mode = current->mode;
                    asm("" : "+r"(current), "+r"(mode) : : "r2", "r3", "r5", "r6");
                }
                if (mode == 2) {
                    {
                        register s32 slotIndex asm("r0") = index;
                        if (slotIndex > 7)
                            goto zeroTarget;
                    }
                    mode = 1;
                    goto storeTarget;
                }
                if (mode != 1)
                    goto minusTarget;
            zeroTarget:
                slot->targetIndex = remaining;
                goto incrementFrame;
            minusTarget:
                mode = 255;
            storeTarget:
                slot->targetIndex = mode;
                goto incrementFrame;
            }
        }
    }
incrementFrame:
    slot->stateFrame++;
done:;
}
