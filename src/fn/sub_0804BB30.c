#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_SetEntry(struct TetherActor *, u32);
void TetherActor_SetHeadingEntry(struct TetherActor *, u32);
void TetherActor_UpdateShake(struct TetherActor *);
void TetherActor_LimitPlayerDistance(struct TetherActor *);
void TetherActor_UpdateLockedShake(struct TetherActor *);
void TetherActor_UpdateSecondaryLock(struct TetherActor *);
static inline s32 absolute(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}
void TetherActor_UpdateFollowingState(struct TetherActor *actor)
{
    if (actor->state != 1)
        TetherActor_SetState(actor, 1);
    if (actor->interactionReset && actor->interactionEnabled) {
        s32 x = actor->position.x;
        s32 differenceX = absolute(x - actor->interactionTarget.x);
        /* The second comparison also uses X in the original. */
        s32 differenceZ = absolute(x - actor->interactionTarget.z);
        if (differenceX > 256 || differenceZ > 256)
            actor->interactionReset = 0;
    }
    actor->glowMode = 0;
    if (actor->parameter >= 0)
        TetherActor_LimitPlayerDistance(actor);
    else {
        register s16 *x asm("r2") = &actor->position.x;
        s16 *z;
        register s32 dx asm("r1") = *x;
        s32 target = actor->targetPosition.x;
        dx = absolute(dx - target);
        z = &actor->position.z;
        if (dx > 2 || absolute(*z - actor->targetPosition.z) > 2)
            actor->moving = 1;
        *x = actor->targetPosition.x;
        *z = actor->targetPosition.z;
    }
    actor->heading = actor->targetPosition.pad;
    if (actor->kind == 5 && actor->moving)
        TetherActor_SetHeadingEntry(actor, 36);
    else
        TetherActor_SetEntry(actor, 0);
    if (!(gUnk_030053F4 & 512) && actor->kind != 9) {
        u16 *lock = &actor->interactionLock;
        if (*lock)
            TetherActor_UpdateLockedShake(actor);
        else {
            u32 ready = actor->chargeReady;
            if (ready)
                TetherActor_UpdateShake(actor);
            else if (actor->blendRequested) {
                actor->secondaryValue = actor->secondaryMaximum;
                actor->currentValue = actor->maximumValue;
                *lock = ready;
            } else
                TetherActor_UpdateSecondaryLock(actor);
        }
    }
}
