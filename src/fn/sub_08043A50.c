// MATCHING_CONCESSION: register-pinning
#include "contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
void ContactActor_UpdateAwaitHide(struct ContactActorPool *pool, struct ContactActor *slot,
                                  s32 index)
{
    if (slot->enter) {
        register struct ContactCollider *collider asm("r2");
        slot->enter = 0;
        collider = &slot->healthCollider;
        collider->flags |= 4;
    }
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (pool->hideRequested) {
        u32 state = 8;
        u32 zero = 0;
        slot->state = state;
        slot->enter = 1;
        slot->stateFrame = zero;
    } else
        slot->stateFrame++;
}
