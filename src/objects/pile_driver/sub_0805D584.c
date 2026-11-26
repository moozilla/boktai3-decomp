#include "objects/pile_driver/pile_driver_subject_playback.h"
void PileDriverSubject_TickResetDelay(struct PileDriverSubject *owner)
{
    Anim_SetSequence(&owner->sequence, owner->sequenceResource, 1, owner->sequenceRow,
                     owner->sequenceVariant);
    PileDriverSubject_StepPlayback(owner);
    owner->unknownE2--;
    if (!owner->unknownE2) {
        owner->positionReset = 0;
        owner->drawable.position = owner->home;
        PileDriverSubject_SetState((void *)owner, 2);
    } else
        PileDriverSubject_ResetPosition(owner);
}
