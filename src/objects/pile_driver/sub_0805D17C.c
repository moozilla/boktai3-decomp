#include "objects/pile_driver/pile_driver_subject_playback.h"
void PileDriverSubject_TickActive(struct PileDriverSubject *owner)
{
    Anim_SetSequence(&owner->sequence, owner->sequenceResource, 3, owner->sequenceRow,
                     owner->sequenceVariant);
    PileDriverSubject_StepPlayback(owner);
    if (owner->unknownEE)
        owner->paletteBank = 0x24E;
    else
        owner->paletteBank = 0x24D;
}
