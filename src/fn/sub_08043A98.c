#include "contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
void ContactActor_UpdateInactive(struct ContactActorPool *pool, struct ContactActor *slot,
                                 s32 index)
{
    if (slot->enter) {
        struct ContactCollider *collider;
        slot->enter = 0;
        collider = &slot->healthCollider;
        collider->flags |= 4;
    }
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    slot->stateFrame++;
}
