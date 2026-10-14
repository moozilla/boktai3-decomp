#include "contact_sequence_actor.h"
#define ContactSequenceActor_Destroy sub_080464F0
void ContactSequenceActor_Destroy(struct ContactSequenceOwner *owner,
                                  struct ContactSequenceActor *actor)
{
    Collider_Remove(&actor->healthCollider);
    Sprite_Remove(&actor->sprite);
}
