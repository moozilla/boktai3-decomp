#include "contact_sequence_actor.h"
void ContactSequenceActor_Destroy(struct ContactSequenceOwner *owner,
                                  struct ContactSequenceActor *actor)
{
    Collider_Remove(&actor->healthCollider);
    Sprite_Remove(&actor->sprite);
}
