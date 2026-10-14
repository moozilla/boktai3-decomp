// MATCHING_CONCESSION: register-pinning
#include "contact_actor.h"
#define ContactActor_UpdateInactive sub_08043A98
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
void ContactActor_UpdateInactive(struct ContactActorPool *pool, struct ContactActor *slot,
                                 s32 index)
{
    if (slot->enter) {
        register struct ContactCollider *collider asm("r2");
        slot->enter = 0;
        collider = &slot->healthCollider;
        collider->flags |= 4;
    }
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    slot->stateFrame++;
}
