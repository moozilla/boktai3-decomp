#include "contact_sequence_actor.h"
#include "player_cue_sequence.h"
u32 ContactSequenceActor_TakeEntryFlag(struct ContactSequenceActor *);
void ContactSequenceActor_SetFlags(struct ContactSequenceActor *, u32);
void ContactSequenceActor_UpdateActive(struct ContactSequenceOwner *parent,
                                       struct ContactSequenceActor *owner)
{
    ContactSequenceActor_TakeEntryFlag(owner);
    PlayerCue_StepSequence(&owner->sprite, &owner->sequence);
    Collider_Refresh(&owner->resourceCollider);
}
