#include "objects/pile_driver/pile_driver_subject_playback.h"
void PileDriverSubject_TickIdle(struct PileDriverSubject *owner)
{
    Anim_SetSequence(&owner->sequence, owner->sequenceResource, 1, owner->sequenceRow,
                     owner->sequenceVariant);
    PileDriverSubject_StepPlayback(owner);
    owner->paletteBank = 0x24C;
}
