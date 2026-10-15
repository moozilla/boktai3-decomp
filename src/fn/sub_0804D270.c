#include "tether_actor.h"
void TetherActor_SetState(struct TetherActor *, u32);
s32 TetherActor_SetEntry(struct TetherActor *, u32);
s32 TetherActor_SetHeadingEntry(struct TetherActor *, u32);
void TetherActor_UpdateLockedShake(struct TetherActor *);
void TetherActor_UpdateShake(struct TetherActor *);
void TetherActor_UpdateReverseDirectedMovement(struct TetherActor *);
void TetherActor_UpdateSecondaryLock(struct TetherActor *);
static inline s32 absolute(s32 value)
{
    if (value < 0)
        return -value;
    return value;
}
void TetherActor_UpdateReverseDirectedState(struct TetherActor *actor)
{
    if (actor->state != 17)
        TetherActor_SetState(actor, 17);
    actor->glowMode = 1;
    actor->position = actor->targetPosition;
    if (absolute(actor->position.x - actor->targetPosition.x) > 2 ||
        absolute(actor->position.z - actor->targetPosition.z) > 2)
        actor->moving = 1;
    actor->heading = actor->targetPosition.pad;
    if (actor->kind == 5 && actor->moving)
        TetherActor_SetHeadingEntry(actor, 36);
    else
        TetherActor_SetEntry(actor, 0);
    if (!(gUnk_030053F4 & 512)) {
        u32 value = actor->interactionLock;
        if (value)
            TetherActor_UpdateLockedShake(actor);
        else if (actor->chargeReady) {
            actor->glowMode = value;
            TetherActor_UpdateShake(actor);
        } else {
            TetherActor_UpdateReverseDirectedMovement(actor);
            TetherActor_UpdateSecondaryLock(actor);
        }
    }
}
