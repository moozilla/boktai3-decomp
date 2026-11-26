#include "objects/pile_driver/pile_driver_subject_playback.h"
void PileDriverSubject_TickDeparting(struct PileDriverSubject *owner)
{
    if (owner->kind) {
        if (owner->sequence.index == 5 && owner->sequence.elapsed == 0)
            owner->paletteBank = 0x24D;
        else if (owner->sequence.index == 6 && owner->sequence.elapsed == 0)
            owner->paletteBank = 0x24C;
        Anim_SetSequence(&owner->sequence, owner->sequenceResource, 4, owner->sequenceRow,
                         owner->sequenceVariant);
        if (PileDriverSubject_StepPlayback(owner)) {
            owner->contactProgress = 0;
            owner->unknownE2 = 90;
            PileDriverSubject_SetState((void *)owner, 6);
        }
    } else {
        if (owner->sequence.index == 5 && owner->sequence.elapsed == 0)
            owner->paletteBank = 0x24C;
        Anim_SetSequence(&owner->sequence, owner->sequenceResource, 4, owner->sequenceRow,
                         owner->sequenceVariant);
        if (PileDriverSubject_StepPlayback(owner)) {
            owner->contactProgress = 0;
            PileDriverSubject_SetState((void *)owner, 2);
        }
    }
}
