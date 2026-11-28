#include "scene/collision/contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
void ContactActor_UpdateInactive(struct ContactActorPool *pool, struct ContactActor *slot,
                                 s32 index)
{
    if (slot->enter) {
        slot->enter = 0;
        ContactCollider_SetFlags(&slot->healthCollider, 4);
    }
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    slot->stateFrame++;
}
