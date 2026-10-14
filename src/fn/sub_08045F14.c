// MATCHING_CONCESSION: register-pinning
#include "contact_sequence_actor.h"
void ContactSequenceActor_SetState(struct ContactSequenceActor *, u32);
void ContactSequenceActor_Deactivate(struct ContactSequenceOwner *owner,
                                     struct ContactSequenceActor *actor)
{
    actor->sprite.flags |= 1;
    ContactSequenceActor_SetState(actor, 0);
    {
        register struct ContactCollider *collider asm("r3") = &actor->healthCollider;
        collider->flags |= 4;
    }
    actor->active = 0;
}
