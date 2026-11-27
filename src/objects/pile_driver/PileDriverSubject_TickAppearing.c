#include "objects/pile_driver/pile_driver_subject_playback.h"
void PileDriverSubject_TickAppearing(struct PileDriverSubject *owner)
{
    if (owner->appearDelay)
        owner->appearDelay--;
    else {
        owner->drawable.flags &= ~1;
        owner->collider.flags &= ~4;
        owner->drawable.priority = 2;
        Anim_SetSequence(&owner->sequence, owner->sequenceResource, 0, owner->sequenceRow,
                         owner->sequenceVariant);
        if (PileDriverSubject_StepPlayback(owner)) {
            Collider_Refresh(&owner->collider);
            PileDriverSubject_SetState((void *)owner, 2);
        }
    }
}
